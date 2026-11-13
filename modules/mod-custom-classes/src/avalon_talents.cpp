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
#include "ChatCommand.h"
#include "CommandScript.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "ScriptMgr.h"

using namespace Acore::ChatCommands;

#include <map>
#include <string>
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
    uint32 requiredPoints = 0;
    std::string name;
};

uint32 const SPEC_FLESHWEAVER = 1301;
uint32 const SPEC_BLOOD = 1302;
uint32 const SPEC_ACCURSED = 1303;
uint32 const SPEC_ETERNAL = 1304;
uint8 const CLASS_BLOODMAGE = 13;
uint8 const FIRST_TALENT_LEVEL = 10;

std::map<uint32, TalentNode> g_nodes;
std::map<uint32, std::vector<uint32>> g_nodesBySpec;

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

/// Carrega o catalogo (world DB) e os comandos de spec/talento.
class ModAvalonTalentSystem : public WorldScript
{
public:
    ModAvalonTalentSystem() : WorldScript("ModAvalonTalentSystem", { WORLDHOOK_ON_STARTUP }) { }

    void OnStartup() override
    {
        QueryResult result = WorldDatabase.Query(
            "SELECT `id`, `spec_id`, `tier`, `column_index`, `max_rank`, `spell`,"
            " `prereq`, `prereq_rank`, `required_points`, `name` FROM `avalon_talent_nodes`");
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
            node.name = fields[9].Get<std::string>();

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
        if (player->GetLevel() < FIRST_TALENT_LEVEL)
        {
            handler->SendSysMessage("Avalon: spec a partir do nivel 10.");
            return false;
        }
        if (!IsKnownSpec(specId))
        {
            handler->SendSysMessage("Avalon: spec invalida (1301 Fleshweaver, 1302 Blood, 1303 Accursed, 1304 Eternal).");
            return false;
        }

        CharacterDatabase.Execute(
            "REPLACE INTO `avalon_spec` (`guid`, `spec`) VALUES ({}, {})", player->GetGUID().GetCounter(), specId);

        LOG_INFO("module", "Avalon: {} escolheu spec {} ({} pontos)",
            player->GetName(), specId, TalentPointsFor(player));
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
        TalentNode const& node = it->second;
        uint64 const guid = player->GetGUID().GetCounter();

        QueryResult specResult = CharacterDatabase.Query(
            "SELECT `spec` FROM `avalon_spec` WHERE `guid` = {}", guid);
        uint32 const chosenSpec = specResult ? specResult->Fetch()[0].Get<uint32>() : 0;
        if (chosenSpec != node.specId)
        {
            handler->PSendSysMessage("Avalon: %s nao esta na spec deste no (%u).",
                player->GetName().c_str(), node.specId);
            return false;
        }

        uint8 const rank = KnownRank(guid, nodeId);
        if (rank >= node.maxRank)
        {
            handler->SendSysMessage("Avalon: no ja esta no rank maximo.");
            return false;
        }

        uint32 const total = TalentPointsFor(player);
        uint32 const spent = SpentPoints(guid);
        if (spent >= total)
        {
            handler->PSendSysMessage("Avalon: sem pontos (%u/%u).", spent, total);
            return false;
        }

        if (node.prereq)
        {
            uint8 const prereqRank = KnownRank(guid, node.prereq);
            if (prereqRank < node.prereqRank)
            {
                handler->PSendSysMessage("Avalon: falta o pre-requisito %u (rank %u).",
                    node.prereq, uint32(node.prereqRank));
                return false;
            }
        }

        if (SpentInSpec(guid, node.specId) < node.requiredPoints)
        {
            handler->PSendSysMessage("Avalon: precisa de %u pontos na spec (tem %u).",
                node.requiredPoints, SpentInSpec(guid, node.specId));
            return false;
        }

        CharacterDatabase.Execute(
            "REPLACE INTO `avalon_talents` (`guid`, `node`, `rank`) VALUES ({}, {}, {})",
            guid, nodeId, uint32(rank + 1));

        LOG_INFO("module", "Avalon: {} aprendeu {} rank {}/{}",
            player->GetName(), node.name, rank + 1, uint32(node.maxRank));
        handler->PSendSysMessage("Avalon: %s aprendeu %s (%u/%u).", player->GetName().c_str(),
            node.name.c_str(), rank + 1, uint32(node.maxRank));
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

void AddAvalonTalentScripts()
{
    new ModAvalonTalentSystem();
    new ModAvalonTalentCommand();
}
