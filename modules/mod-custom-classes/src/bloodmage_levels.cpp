/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "PlayerScript.h"
#include "WorldScript.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Player.h"
#include "Config.h"
#include "Log.h"
#include "DatabaseEnv.h"
#include "ObjectGuid.h"
#include "ScriptMgr.h"

#include <set>
#include <unordered_map>
#include <vector>

#include "mod_avalon_classes.h"

namespace
{
/// Primeira classe Avalon com progressao de spells por spec (id 13).
uint8 const BLOODMAGE_CLASS = 13;

/// Uma linha de `avalon_class_spells`: o catalogo do kit de classe.
struct ClassSpell
{
    uint32 spell = 0;
    uint32 specId = 0;
    uint32 level = 0;
};

/// O kit de classe inteiro (world DB), carregado uma vez no startup.
///
/// Antes cada `ClassSpellsFor`/`ClassSpellCatalogue` era uma consulta ao world
/// DB, e as duas rodam no reconcile — que acontece no login, na troca de spec e
/// a cada nivel. Catalogo nao muda em runtime: uma leitura no boot resolve.
std::vector<ClassSpell> g_classSpells;

/// spell -> spec_id, da mesma leitura. O filtro da lista do trainer precisa
/// desta projecao; manter as duas no mesmo lugar evita duas leituras da mesma
/// tabela em dois arquivos.
std::unordered_map<uint32, uint32> g_classSpellSpecs;

/// Spells do kit de classe permitidos pela spec e pelo nivel.
/// `spec_id = 0` e' base da classe e vale para qualquer spec.
std::set<uint32> ClassSpellsFor(uint32 specId, uint32 level)
{
    std::set<uint32> spells;
    for (ClassSpell const& entry : g_classSpells)
        if ((entry.specId == 0 || entry.specId == specId) && entry.level <= level)
            spells.insert(entry.spell);
    return spells;
}

/// Todo spell que o kit de classe pode conceder, sem filtro de spec/nivel.
/// E' o limite de remocao: so' spell daqui (ou do catalogo de talentos) sai.
std::set<uint32> ClassSpellCatalogue()
{
    std::set<uint32> spells;
    for (ClassSpell const& entry : g_classSpells)
        spells.insert(entry.spell);
    return spells;
}
} // namespace

void LoadClassSpellCatalogue()
{
    g_classSpells.clear();
    g_classSpellSpecs.clear();

    QueryResult result = WorldDatabase.Query(
        "SELECT `spell`, `spec_id`, `level` FROM `avalon_class_spells` WHERE `class` = {}",
        uint32(BLOODMAGE_CLASS));
    if (!result)
    {
        LOG_ERROR("module", "Avalon: avalon_class_spells vazia para a classe {}; kit desativado",
            uint32(BLOODMAGE_CLASS));
        return;
    }

    do
    {
        Field* fields = result->Fetch();
        ClassSpell entry;
        entry.spell = fields[0].Get<uint32>();
        entry.specId = fields[1].Get<uint32>();
        entry.level = fields[2].Get<uint32>();
        g_classSpells.push_back(entry);
        g_classSpellSpecs[entry.spell] = entry.specId;
    } while (result->NextRow());

    LOG_INFO("module", "Avalon: {} spells de kit carregadas para a classe {}",
        uint32(g_classSpells.size()), uint32(BLOODMAGE_CLASS));
}

std::unordered_map<uint32, uint32> const& ClassSpellSpecs()
{
    return g_classSpellSpecs;
}

/// Dono unico de "spells que o personagem deve ter": kit de classe permitido
/// pela spec/nivel **uniao** as passivas dos nos de talento com rank.
///
/// Concede o que falta e remove o que deixou de valer. A remocao se limita ao
/// catalogo do modulo (kit de classe + catalogo de talentos): recompensa de
/// quest, compra de trainer fora do kit ou racial nunca sao tocadas.
///
/// `specId` e `rankedTalents` valem como dados. A sobrecarga de um argumento
/// le' os dois do banco; os chamadores que acabaram de gravar spec ou limpar
/// ranks usam esta forma, porque `Execute` e' assincrono e a releitura poderia
/// devolver o estado anterior.
void ReconcileSpells(Player* player, uint32 specId, std::set<uint32> const& rankedTalents)
{
    if (!player || player->getClass() != BLOODMAGE_CLASS)
        return;

    // O que o personagem DEVE ter. Por construcao allowed e' subconjunto de
    // catalogue (mesma classe / mesmos nos), entao iterar o catalogo cobre os
    // dois sentidos.
    std::set<uint32> allowed = ClassSpellsFor(specId, player->GetLevel());
    allowed.insert(rankedTalents.begin(), rankedTalents.end());

    // O que o modulo PODE conceder.
    std::set<uint32> catalogue = ClassSpellCatalogue();
    std::set<uint32> const talentCatalogue = AllTalentSpells();
    catalogue.insert(talentCatalogue.begin(), talentCatalogue.end());

    uint32 granted = 0;
    uint32 removed = 0;
    for (uint32 spellId : catalogue)
    {
        bool const has = player->HasSpell(spellId);
        bool const should = allowed.count(spellId) != 0;

        if (should && !has)
        {
            player->learnSpell(spellId, false);
            ++granted;
        }
        else if (!should && has)
        {
            // Mesma remocao que o Eluna RemoveSpell fazia (spec mask completa).
            player->removeSpell(spellId, SPEC_MASK_ALL, false);
            ++removed;
        }
    }

    if (granted || removed)
    {
        player->SaveToDB(false, false);
        LOG_INFO("module", "Avalon: reconcile de {} (spec {}, nivel {}): {} concedidas, {} removidas",
            player->GetName(), specId, uint32(player->GetLevel()), granted, removed);
    }
}

/// Le' spec e ranks do banco. Seguro onde nao ha escrita pendente (login,
/// subida de nivel).
void ReconcileSpells(Player* player)
{
    if (!player || player->getClass() != BLOODMAGE_CLASS)
        return;

    uint64 const guid = player->GetGUID().GetCounter();
    ReconcileSpells(player, ChosenSpec(guid), TalentSpellsForGuid(guid));
}

class ModAvalonBloodmageProgressao : public PlayerScript
{
public:
    ModAvalonBloodmageProgressao() :
        PlayerScript("ModAvalonBloodmageProgressao",
        {
            PLAYERHOOK_ON_LEVEL_CHANGED,
        })
    {
        LOG_INFO("module", "ModAvalonBloodmageProgressao loaded — bloodmage level progression");
    }

    void OnPlayerLevelChanged(Player* player, uint8 /*oldLevel*/) override
    {
        // Subir de nivel concede o que o nivel liberou e mantem as passivas de
        // talento ja' aprendidas (elas entram na uniao do reconcile).
        ReconcileSpells(player);
    }
};

/// Carrega o catalogo do kit de classe uma vez, no boot.
class ModAvalonClassSpellCatalogue : public WorldScript
{
public:
    ModAvalonClassSpellCatalogue() :
        WorldScript("ModAvalonClassSpellCatalogue", { WORLDHOOK_ON_STARTUP }) { }

    void OnStartup() override
    {
        LoadClassSpellCatalogue();
    }
};

void AddAvalonBloodmageProgressaoScripts()
{
    new ModAvalonBloodmageProgressao();
    new ModAvalonClassSpellCatalogue();
}
