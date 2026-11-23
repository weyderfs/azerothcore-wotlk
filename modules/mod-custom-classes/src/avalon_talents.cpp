/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

// Sistema de spec e talentos do Avalon (Plano 7).
//
// O cliente desenha a arvore a partir do proprio catalogo; o servidor e a
// fonte de verdade: spec escolhida, ranks aprendidos, pontos e pre-requisitos.
// Como o chat da race 12 ainda esta bloqueado (GAPS C-24), os comandos aceitam
// jogador por nome e funcionam pelo console/SOAP.

#include "Chat.h"
#include "GossipDef.h"
#include "ScriptedGossip.h"
#include "ChatCommand.h"
#include "CommandScript.h"
#include "DatabaseEnv.h"
#include "GameTime.h"
#include "Log.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "ScriptMgr.h"

#include "mod_avalon_classes.h"

using namespace Acore::ChatCommands;

#include <map>
#include <mutex>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

namespace
{
struct TalentNode
{
    uint32 id = 0;
    uint32 specId = 0;
    uint8 tier = 0;
    uint8 column = 0;
    uint8 maxRank = 0;
    uint32 spell = 0;
    uint32 prereq = 0;
    uint8 prereqRank = 0;
    uint32 prereq2 = 0;
    uint8 prereqRank2 = 0;
    uint32 requiredPoints = 0;
    std::string name;
};

// Declaracoes antecipadas: os helpers de gossip e de comando usam estes.
uint32 SpentPoints(uint64 guid);
uint8 KnownRank(uint64 guid, uint32 nodeId);
uint32 SpentInSpec(uint64 guid, uint32 specId);

/// Le' o inteiro decimal no inicio de `text` (0 se nao houver digito). Usado
/// pelo canal AVALONX nas cargas "<VERB>:<id>".
uint32 ParseLeadingUint(std::string const& text)
{
    uint32 value = 0;
    for (char c : text)
    {
        if (c < '0' || c > '9')
            break;
        value = value * 10 + uint32(c - '0');
    }
    return value;
}

uint32 const SPEC_FLESHWEAVER = 1301;
uint32 const SPEC_BLOOD = 1302;
uint32 const SPEC_ACCURSED = 1303;
uint32 const SPEC_ETERNAL = 1304;
uint8 const CLASS_BLOODMAGE = 13;
uint8 const FIRST_TALENT_LEVEL = 10;

std::map<uint32, TalentNode> g_nodes;
std::map<uint32, std::vector<uint32>> g_nodesBySpec;

/// Janela do throttle dos canais Avalon (AVALONX e gossip do NPC), em
/// milissegundos: no maximo uma acao processada por personagem a cada janela.
/// 100 ms = 10/s. O canal addon nao tem teto proprio no core: o branch
/// LANG_ADDON pula o gate de mute e o teto que sobra e'
/// `ChatFlood.AddonMessageCount` = 100 por `ChatFlood.MessageDelay` = 1 s — 10x
/// mais frouxo que isto. Entao 10/s e' deliberadamente 10x mais apertado que o
/// teto real do canal, e mesmo assim generoso: um clique de talento e' uma acao
/// discreta atras de um round-trip e de um redraw, entao um humano fica bem
/// abaixo de 10/s. Um loop de flood fica preso a ~10 conexoes de banco por
/// segundo por sessao em vez de milhares.
uint32 const AVALON_CHANNEL_COOLDOWN_MS = 100;

/// Instante (ms de jogo) da ultima acao processada, por guid low do personagem.
/// Bounded, mas nao "so' os online": `OnPlayerLogout` so' roda quando
/// `!redirecting` (WorldSession.cpp:833), entao uma sessao destruida por outro
/// caminho deixa a entrada. O conjunto e' no maximo os personagens que ja'
/// logaram neste processo; cada entrada e' sobrescrita no proximo allow e
/// apagada no proximo logout.
std::unordered_map<ObjectGuid::LowType, uint32> g_avalonChannelLastMs;

/// Os pontos de entrada do modulo que escrevem talento nao rodam todos na mesma
/// thread: o addon-message (`CMSG_MESSAGECHAT`) e o gossip
/// (`CMSG_GOSSIP_SELECT_OPTION`) sao PROCESS_THREADUNSAFE (thread do mundo), mas
/// o opcode `CMSG_AVALON_TALENT` e' PROCESS_INPLACE e pode ser processado na
/// thread de mapa. Por isso a tabela e' protegida; a secao critica e' um
/// lookup + insert, sem banco e sem I/O.
std::mutex g_avalonChannelMutex;

/// true quando a acao pode ser processada agora; false para descartar.
bool AvalonChannelAllowed(Player* player)
{
    uint32 const now = uint32(GameTime::GetGameTimeMS().count());
    ObjectGuid::LowType const guid = player->GetGUID().GetCounter();

    std::lock_guard<std::mutex> lock(g_avalonChannelMutex);
    auto it = g_avalonChannelLastMs.find(guid);
    if (it != g_avalonChannelLastMs.end() && now - it->second < AVALON_CHANNEL_COOLDOWN_MS)
        return false;

    g_avalonChannelLastMs[guid] = now;
    return true;
}

/// Esquece o throttle de um personagem para a tabela nao crescer no logout.
void AvalonChannelForget(Player* player)
{
    ObjectGuid::LowType const guid = player->GetGUID().GetCounter();

    std::lock_guard<std::mutex> lock(g_avalonChannelMutex);
    g_avalonChannelLastMs.erase(guid);
}

bool IsKnownSpec(uint32 specId)
{
    return specId == SPEC_FLESHWEAVER || specId == SPEC_BLOOD ||
           specId == SPEC_ACCURSED || specId == SPEC_ETERNAL;
}

uint32 TalentPointsFor(Player* player)
{
    if (!player || player->GetLevel() <= FIRST_TALENT_LEVEL)
        return player && player->GetLevel() == FIRST_TALENT_LEVEL ? 1u : 0u;
    return uint32(player->GetLevel()) - (FIRST_TALENT_LEVEL - 1);
}

/// Os pre-requisitos de um no' sao alternativas (OU), nao exigencias
/// cumulativas: bloqueia so' quando o no' declara ao menos um pre-requisito e
/// nenhum deles esta satisfeito; no' sem pre-requisitos sempre passa.
/// Os pares alternativos vem do kit de referencia; neste catalogo, 64 nos,
/// 12 deles com dois pre-requisitos.
bool PrerequisitesMet(uint64 guid, TalentNode const& node)
{
    auto satisfied = [&](uint32 prereq, uint8 rank)
    {
        return prereq != 0 && KnownRank(guid, prereq) >= rank;
    };

    return !(node.prereq || node.prereq2) ||
           satisfied(node.prereq, node.prereqRank) || satisfied(node.prereq2, node.prereqRank2);
}

bool LearnNode(Player* player, uint32 nodeId, std::string& error, uint8* newRank = nullptr)
{
    auto it = g_nodes.find(nodeId);
    if (it == g_nodes.end())
    {
        error = "No inexistente.";
        return false;
    }

    TalentNode const& node = it->second;
    uint64 const guid = player->GetGUID().GetCounter();

    if (ChosenSpec(guid) != node.specId)
    {
        error = "Este no pertence a outra spec.";
        return false;
    }

    uint8 const rank = KnownRank(guid, nodeId);
    if (rank >= node.maxRank)
    {
        error = "Rank maximo atingido.";
        return false;
    }

    uint32 const total = TalentPointsFor(player);
    uint32 const spent = SpentPoints(guid);
    if (spent >= total)
    {
        error = "Sem pontos disponiveis.";
        return false;
    }

    if (!PrerequisitesMet(guid, node))
    {
        error = "Pre-requisito nao atendido.";
        return false;
    }

    if (SpentInSpec(guid, node.specId) < node.requiredPoints)
    {
        error = "Pontos na spec insuficientes.";
        return false;
    }

    // Escrita sincrona: o learn pela janela e' seguido por um RequestState do
    // cliente; com `Execute` (assincrono) essa releitura podia ver o rank antigo.
    CharacterDatabase.DirectExecute(
        "REPLACE INTO `avalon_talents` (`guid`, `node`, `rank`) VALUES ({}, {}, {})",
        guid, nodeId, uint32(rank + 1));

    // A passiva e' spell real com SPELL_ATTR0_PASSIVE: o core casta no learn
    // (Player.cpp:3335) e a aura sobrevive a relog. Sem passiva declarada
    // (spell = 0) o no' e' so' catalogo, como os 64 do alpha.
    if (node.spell && !player->HasSpell(node.spell))
        player->learnSpell(node.spell, false);

    if (newRank)
        *newRank = rank + 1;

    LOG_INFO("module", "Avalon: {} aprendeu {} rank {}/{}",
        player->GetName(), node.name, rank + 1, uint32(node.maxRank));
    return true;
}

/// Apaga os ranks da spec anterior. Os ids vem do catalogo em memoria: a
/// subquery com `avalon_talent_nodes` (world DB) a partir da conexao de
/// personagens nao resolve — a tabela nao existe no character DB.
void DeleteSpecNodes(uint64 guid, uint32 specId)
{
    auto const it = g_nodesBySpec.find(specId);
    if (it == g_nodesBySpec.end() || it->second.empty())
        return;

    std::string ids;
    for (uint32 nodeId : it->second)
    {
        if (!ids.empty())
            ids += ',';
        ids += std::to_string(nodeId);
    }

    CharacterDatabase.DirectExecute(
        "DELETE FROM `avalon_talents` WHERE `guid` = {} AND `node` IN ({})", guid, ids);
}

bool SetSpec(Player* player, uint32 specId, std::string& error)
{
    if (!IsKnownSpec(specId))
    {
        error = "Spec invalida.";
        return false;
    }
    if (player->GetLevel() < FIRST_TALENT_LEVEL)
    {
        error = "Spec a partir do nivel 10.";
        return false;
    }
    uint64 const guid = player->GetGUID().GetCounter();
    uint32 const previous = ChosenSpec(guid);
    CharacterDatabase.DirectExecute(
        "REPLACE INTO `avalon_spec` (`guid`, `spec`) VALUES ({}, {})", guid, specId);

    // Passivas com rank depois da troca: spec nova nasce sem arvore, entao o
    // conjunto e' vazio; trocar para a mesma spec preserva a arvore atual.
    std::set<uint32> rankedTalents;
    if (previous && previous != specId)
    {
        // Talentos sao por spec: trocar de spec zera apenas a arvore da spec anterior.
        DeleteSpecNodes(guid, previous);
        LOG_INFO("module", "Avalon: {} trocou de spec {} -> {} (talentos zerados)",
            player->GetName(), previous, specId);
    }
    else
    {
        rankedTalents = TalentSpellsForGuid(guid);
    }

    // Escrita sincrona (DirectExecute): a spec e a arvore ja' estao no banco
    // quando este ponto roda. O estado explicito evita qualquer releitura.
    ReconcileSpells(player, specId, rankedTalents);

    LOG_INFO("module", "Avalon: {} escolheu spec {}", player->GetName(), specId);
    return true;
}

/// Zera a arvore de talentos e as passivas com rank, mantendo a spec. Mesma
/// acao do gossip 900; o canal AVALONX reusa esta funcao em vez de repetir o
/// DELETE. Escrita sincrona para que o RequestState que a janela manda logo
/// depois enxergue a arvore ja' vazia.
void ResetTalents(Player* player)
{
    uint64 const guid = player->GetGUID().GetCounter();
    CharacterDatabase.DirectExecute("DELETE FROM `avalon_talents` WHERE `guid` = {}", guid);
    LOG_INFO("module", "Avalon: {} resetou os talentos (spec mantida)", player->GetName());
    // Arvore vazia: as passivas com rank depois do reset sao o conjunto vazio.
    ReconcileSpells(player, ChosenSpec(guid), std::set<uint32>());
}

Player* ResolvePlayer(ChatHandler* handler, std::string const& name)
{
    if (Player* self = handler->GetPlayer())
    {
        if (name.empty() || self->GetName() == name)
            return self;
    }
    return ObjectAccessor::FindPlayerByName(name);
}

/// Pontos gastos = soma dos ranks aprendidos.
uint32 SpentPoints(uint64 guid)
{
    QueryResult result = CharacterDatabase.Query(
        "SELECT COALESCE(SUM(`rank`), 0) FROM `avalon_talents` WHERE `guid` = {}", guid);
    if (!result)
        return 0;
    return result->Fetch()[0].Get<uint32>();
}

uint8 KnownRank(uint64 guid, uint32 nodeId)
{
    QueryResult result = CharacterDatabase.Query(
        "SELECT `rank` FROM `avalon_talents` WHERE `guid` = {} AND `node` = {}", guid, nodeId);
    return result ? result->Fetch()[0].Get<uint8>() : 0;
}

uint32 SpentInSpec(uint64 guid, uint32 specId)
{
    uint32 spent = 0;
    for (uint32 nodeId : g_nodesBySpec[specId])
        spent += KnownRank(guid, nodeId);
    return spent;
}
} // namespace

/// Spec ativa do personagem (tabela `avalon_spec`, characters DB). 0 = nenhuma.
/// Fora do namespace anonimo: `avalon_trainer.cpp` e `bloodmage_levels.cpp`
/// usam a mesma leitura (declarada em mod_avalon_classes.h).
uint32 ChosenSpec(uint64 guid)
{
    QueryResult result = CharacterDatabase.Query(
        "SELECT `spec` FROM `avalon_spec` WHERE `guid` = {}", guid);
    return result ? result->Fetch()[0].Get<uint32>() : 0;
}

/// Passivas que aquele personagem deve ter pelos nos de talento com rank > 0.
/// Le' os ranks do character DB e mapeia pelo catalogo ja' carregado. Fora do
/// namespace anonimo: `bloodmage_levels.cpp` chama isto no reconcile.
std::set<uint32> TalentSpellsForGuid(uint64 guid)
{
    std::set<uint32> spells;

    // So' ranks da spec ativa contam: `LearnNode` exige que o no' seja da spec
    // do personagem, entao um rank de outra spec so' existe em estado
    // inconsistente (ex.: troca de spec por um transporte que nao zerou a
    // arvore). Ignora-lo impede que a passiva orfa volte a ser concedida.
    uint32 const activeSpec = ChosenSpec(guid);

    QueryResult rows = CharacterDatabase.Query(
        "SELECT `node` FROM `avalon_talents` WHERE `guid` = {} AND `rank` > 0", guid);
    if (!rows)
        return spells;

    do
    {
        uint32 const nodeId = rows->Fetch()[0].Get<uint32>();
        auto it = g_nodes.find(nodeId);
        if (it != g_nodes.end() && it->second.spell && it->second.specId == activeSpec)
            spells.insert(it->second.spell);
    } while (rows->NextRow());

    return spells;
}

/// Toda passiva declarada no catalogo de talentos. O reconcile usa isto como
/// limite de remocao: so' spells daqui (ou do kit de classe) podem sair.
std::set<uint32> AllTalentSpells()
{
    std::set<uint32> spells;
    for (auto const& entry : g_nodes)
        if (entry.second.spell)
            spells.insert(entry.second.spell);
    return spells;
}

/// Carrega o catalogo (world DB) e os comandos de spec/talento.
class ModAvalonTalentSystem : public WorldScript
{
public:
    ModAvalonTalentSystem() : WorldScript("ModAvalonTalentSystem", { WORLDHOOK_ON_STARTUP }) { }

    void OnStartup() override
    {
        QueryResult result = WorldDatabase.Query(
            "SELECT `id`, `spec_id`, `tier`, `column_index`, `max_rank`, `spell`,"
            " `prereq`, `prereq_rank`, `required_points`, `prereq2`, `prereq_rank2`,"
            " `name` FROM `avalon_talent_nodes`");
        if (!result)
        {
            LOG_ERROR("module", "Avalon: avalon_talent_nodes vazia; talentos desativados");
            return;
        }

        g_nodes.clear();
        g_nodesBySpec.clear();
        do
        {
            Field* fields = result->Fetch();
            TalentNode node;
            node.id = fields[0].Get<uint32>();
            node.specId = fields[1].Get<uint32>();
            node.tier = fields[2].Get<uint8>();
            node.column = fields[3].Get<uint8>();
            node.maxRank = fields[4].Get<uint8>();
            node.spell = fields[5].Get<uint32>();
            node.prereq = fields[6].Get<uint32>();
            node.prereqRank = fields[7].Get<uint8>();
            node.requiredPoints = fields[8].Get<uint32>();
            node.prereq2 = fields[9].Get<uint32>();
            node.prereqRank2 = fields[10].Get<uint8>();
            node.name = fields[11].Get<std::string>();   // era fields[9]

            g_nodes[node.id] = node;
            g_nodesBySpec[node.specId].push_back(node.id);
        } while (result->NextRow());

        LOG_INFO("module", "Avalon: {} nos de talento carregados em {} specs",
            g_nodes.size(), g_nodesBySpec.size());
    }
};

class ModAvalonTalentCommand : public CommandScript
{
public:
    ModAvalonTalentCommand() : CommandScript("ModAvalonTalentCommand") { }

    Acore::ChatCommands::ChatCommandTable GetCommands() const override
    {
        static Acore::ChatCommands::ChatCommandTable avalonTable =
        {
            { "spec",   HandleSpecCommand,   rbac::RBAC_PERM_COMMAND_GM, Console::Yes },
            { "talent", HandleTalentCommand, rbac::RBAC_PERM_COMMAND_GM, Console::Yes },
            { "info",   HandleInfoCommand,   rbac::RBAC_PERM_COMMAND_GM, Console::Yes },
        };
        static Acore::ChatCommands::ChatCommandTable root = { { "avalon", avalonTable } };
        return root;
    }

private:
    static bool HandleSpecCommand(ChatHandler* handler, std::string const& playerName, uint32 specId)
    {
        Player* player = ResolvePlayer(handler, playerName);
        if (!player)
        {
            handler->SendSysMessage("Avalon: jogador nao encontrado (precisa estar online).");
            return false;
        }
        if (player->getClass() != CLASS_BLOODMAGE)
        {
            handler->SendSysMessage("Avalon: spec so existe para Bloodmage.");
            return false;
        }

        // A regra (spec valida, nivel, troca de arvore, spells de classe e
        // passivas) e' do SetSpec, dono unico. O comando so' resolve o jogador
        // e formata a saida.
        std::string error;
        if (!SetSpec(player, specId, error))
        {
            handler->SendSysMessage(error);
            return false;
        }

        handler->PSendSysMessage("Avalon: spec de %s definida para %u.", player->GetName().c_str(), specId);
        return true;
    }

    static bool HandleTalentCommand(ChatHandler* handler, std::string const& playerName, uint32 nodeId)
    {
        Player* player = ResolvePlayer(handler, playerName);
        if (!player)
        {
            handler->SendSysMessage("Avalon: jogador nao encontrado (precisa estar online).");
            return false;
        }

        auto it = g_nodes.find(nodeId);
        if (it == g_nodes.end())
        {
            handler->SendSysMessage("Avalon: no inexistente.");
            return false;
        }

        // A regra (spec, rank, pontos, pre-requisito, required_points, persistencia
        // e a passiva) e' do LearnNode, dono unico. A mensagem de sucesso usa o
        // rank que o proprio LearnNode devolve em `newRank`.
        uint8 newRank = 0;
        std::string error;
        if (!LearnNode(player, nodeId, error, &newRank))
        {
            handler->SendSysMessage(error);
            return false;
        }

        handler->PSendSysMessage("Avalon: %s aprendeu %s (%u/%u).", player->GetName().c_str(),
            it->second.name.c_str(), uint32(newRank), uint32(it->second.maxRank));
        return true;
    }

    static bool HandleInfoCommand(ChatHandler* handler, std::string const& playerName)
    {
        Player* player = ResolvePlayer(handler, playerName);
        if (!player)
        {
            handler->SendSysMessage("Avalon: jogador nao encontrado (precisa estar online).");
            return false;
        }
        uint64 const guid = player->GetGUID().GetCounter();

        QueryResult specResult = CharacterDatabase.Query(
            "SELECT `spec` FROM `avalon_spec` WHERE `guid` = {}", guid);
        uint32 const spec = specResult ? specResult->Fetch()[0].Get<uint32>() : 0;

        handler->PSendSysMessage("Avalon: %s nivel %u spec %u pontos %u/%u",
            player->GetName().c_str(), uint32(player->GetLevel()), spec,
            SpentPoints(guid), TalentPointsFor(player));
        return true;
    }
};


/// Canal proprio do cliente: a janela manda spec, reset e learn por aqui.
///
/// Dois gatilhos:
///   a) hook do core (CMSG 0x449 reescrito pela extensao)
///   b) addon-message: o addon manda "AVALONX<TAB>LEARN:<id>" (ou SETSPEC:<id>,
///      ou RESET) como WHISPER; o core chama OnPlayerBeforeSendChatMessage
///      antes de rotear, e o modulo processa. Este caminho funciona sem DLL.
class ModAvalonTalentChannel : public PlayerScript
{
public:
    ModAvalonTalentChannel() : PlayerScript("ModAvalonTalentChannel",
        {
            PLAYERHOOK_ON_AVALON_TALENT_LEARN,
            PLAYERHOOK_ON_BEFORE_SEND_CHAT_MESSAGE,
            PLAYERHOOK_ON_LOGIN,
            PLAYERHOOK_ON_LOGOUT,
        }) { }

    void OnPlayerLogin(Player* player) override
    {
        // O estado da arvore e' enviado pelo script AIO (lua-scripts); o que o
        // personagem deve ter no livro de spells e' reconciliado aqui, no
        // ponto de entrada do login. Cobre qualquer escrita que nao chegou ao
        // banco antes do ultimo logout ou crash.
        ReconcileSpells(player);
    }

    void OnPlayerLogout(Player* player) override
    {
        // Sem isto a tabela de throttle cresceria a cada sessao que passa.
        if (player)
            AvalonChannelForget(player);
    }

    void OnPlayerBeforeSendChatMessage(Player* player, uint32& type, uint32& lang, std::string& msg) override
    {
        if (!player || player->getClass() != CLASS_BLOODMAGE)
            return;
        if (lang != LANG_ADDON)
            return;
        if (msg.rfind("AVALONX", 0) != 0)
            return;

        // SendAddonMessage entrega "PREFIXO<TAB>mensagem"; aceitar '|' tambem
        // para nao depender do cliente.
        std::string::size_type sep = msg.find('\t');
        if (sep == std::string::npos)
            sep = msg.find('|');
        if (sep == std::string::npos)
            return;

        // Throttle do canal: um flood de addon-message nao pode virar uma
        // conexao de banco por mensagem na thread do mundo. Throttled e'
        // dropado em silencio; nao ha' estado obsoleto a corrigir porque a
        // janela relê o estado real via RequestState logo depois.
        if (!AvalonChannelAllowed(player))
            return;

        std::string const payload = msg.substr(sep + 1);
        LOG_INFO("module", "Avalon: addon msg de {}: {}", player->GetName(), payload);

        // Os tres verbos que a janela manda; a leitura de estado (RequestState,
        // ViewSpec) continua no script AIO, que nao muda nada.
        std::string error;
        if (payload.rfind("LEARN:", 0) == 0)
        {
            if (uint32 const nodeId = ParseLeadingUint(payload.substr(6)))
                if (!LearnNode(player, nodeId, error))
                    ChatHandler(player->GetSession()).SendSysMessage(error, true);
        }
        else if (payload.rfind("SETSPEC:", 0) == 0)
        {
            if (uint32 const specId = ParseLeadingUint(payload.substr(8)))
                if (!SetSpec(player, specId, error))
                    ChatHandler(player->GetSession()).SendSysMessage(error, true);
        }
        else if (payload == "RESET")
        {
            ResetTalents(player);
        }
    }

    bool OnAvalonTalentLearn(Player* player, uint32 nodeId) override
    {
        if (!player || player->getClass() != CLASS_BLOODMAGE)
            return false;

        // Segundo ponto de entrada do mesmo LearnNode (`CMSG_AVALON_TALENT`,
        // PROCESS_INPLACE): precisa do mesmo throttle do addon-message, senao o
        // flood so' troca de opcode.
        if (!AvalonChannelAllowed(player))
            return false;

        std::string error;
        if (!LearnNode(player, nodeId, error))
        {
            ChatHandler(player->GetSession()).SendSysMessage(error, true);
            return false;
        }
        return true;
    }
};

/// NPC de talentos: escolher spec e gastar pontos sem depender de chat.
class npc_avalon_talents : public CreatureScript
{
public:
    npc_avalon_talents() : CreatureScript("npc_avalon_talents") { }

    bool OnGossipHello(Player* player, Creature* creature) override
    {
        ShowMenu(player, creature);
        return true;
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        player->PlayerTalkClass->ClearMenus();

        // O gossip tem o mesmo custo do canal AVALONX (SetSpec/ResetTalents usam
        // DirectExecute) e nao tem policy AntiDOS para 0x17C; divide o mesmo
        // throttle por guid. So' as acoes de escrita consomem: a acao 500 e' a
        // lista do trainer (leitura) e nao passa por aqui. Throttled = dropa a
        // escrita em silencio e reenvia o menu; o menu ja' reflete o banco, entao
        // nao ha' estado obsoleto a corrigir.
        std::string error;
        if (action >= 1 && action <= 4)
        {
            if (AvalonChannelAllowed(player) && !SetSpec(player, 1300 + action, error))
                ChatHandler(player->GetSession()).SendSysMessage(error, true);
        }
        else if (action == 500)
        {
            player->GetSession()->SendTrainerList(creature);
            return true;
        }
        else if (action == 900)
        {
            // Reset e' repetivel: o menu reabre depois de cada select.
            if (AvalonChannelAllowed(player))
                ResetTalents(player);
        }

        ShowMenu(player, creature);
        return true;
    }

private:
    static void ShowMenu(Player* player, Creature* creature)
    {
        player->PlayerTalkClass->ClearMenus();

        if (player->getClass() != CLASS_BLOODMAGE)
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Only Bloodmages may study these rites.", GOSSIP_SENDER_MAIN, 99);
            SendGossipMenuFor(player, 900000, creature);
            return;
        }

        uint32 const spec = ChosenSpec(player->GetGUID().GetCounter());
        if (!spec)
        {
            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Fleshweaver - mend and pool vitality", GOSSIP_SENDER_MAIN, 1);
            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Blood - spend health to destroy", GOSSIP_SENDER_MAIN, 2);
            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Accursed - let the beast fight", GOSSIP_SENDER_MAIN, 3);
            AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Eternal - leech and outlast", GOSSIP_SENDER_MAIN, 4);
            SendGossipMenuFor(player, 900000, creature);
            return;
        }

        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "I am ready for training.", GOSSIP_SENDER_MAIN, 500);
        AddGossipItemFor(player, GOSSIP_ICON_TRAINER, "Change my specialization.", GOSSIP_SENDER_MAIN, 900);
        SendGossipMenuFor(player, 900000, creature);
    }
};

void AddAvalonTalentScripts()
{
    new ModAvalonTalentSystem();
    new ModAvalonTalentCommand();
    new ModAvalonTalentChannel();
    new npc_avalon_talents();
}
