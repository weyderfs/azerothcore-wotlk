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
#include "Log.h"
#include "Player.h"
#include "Config.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "WorldSessionMgr.h"
#include "SpellAuras.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "SpellScriptLoader.h"

#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "mod_avalon_classes.h"

class ModAvalonClasses : public PlayerScript
{
public:
    ModAvalonClasses() :
        PlayerScript("ModAvalonClasses",
        {
            PLAYERHOOK_ON_LOGIN,
        })
    {
        LOG_INFO("module", "ModCustomClasses module loaded successfully");
    }

    void OnPlayerLogin(Player* player) override
    {
        HandleAvalonClass(player);
    }

private:
    /// Verifica se o jogador tem a classe Avalon e aplica passivas/inicialização
    void HandleAvalonClass(Player* player)
    {
        if (!player || player->getClass() != AVALON_CLASS_ID)
            return;

        LOG_INFO("module", "Player {} ({}) has Avalon class ID {}, applying initialization",
            player->GetName(), player->GetGUID().ToString(), AVALON_CLASS_ID);

        // Aqui serão aplicadas as passivas, equipamento inicial e atributos da classe Avalon
        // quando a Fase 2 (DBCs) e a injeção de SQL estiverem concluídas
    }
};

/// Probe do protocolo: envia AVALON_SMSG_PROBE para sessoes no char select.
/// Prova que o cliente com avalon-ext.dll recebe um opcode fora do range stock.
class ModAvalonProbe : public WorldScript
{
public:
    ModAvalonProbe() :
        WorldScript("ModAvalonProbe", { WORLDHOOK_ON_UPDATE })
    {
        _enabled = sConfigMgr->GetOption<bool>("Avalon.Probe.Enable", false);
        _interval = sConfigMgr->GetOption<uint32>("Avalon.Probe.Interval", 5000);
        LOG_INFO("module", "ModAvalonProbe {} (interval {} ms)",
            _enabled ? "enabled" : "disabled", _interval);
    }

    void OnUpdate(uint32 diff) override
    {
        if (!_enabled)
            return;

        _timer += diff;
        if (_timer < _interval)
            return;
        _timer = 0;

        for (auto const& pair : sWorldSessionMgr->GetAllSessions())
        {
            WorldSession* session = pair.second;
            if (!session || session->IsSocketClosed() || session->GetPlayer())
                continue;

            std::string const text = "AVALON-PROBE";
            WorldPacket packet(AVALON_SMSG_PROBE, 8 + text.size());
            packet << uint32(text.size());
            packet.append(text.data(), text.size());
            packet << uint32(++_counter);
            session->SendPacket(&packet);

            LOG_INFO("module", "avalon probe #{} enviado para session {}", _counter, pair.first);
        }
    }

private:
    bool _enabled = true;
    uint32 _interval = 5000;
    uint32 _timer = 0;
    uint32 _counter = 0;
};

/// Spells iniciais por classe Avalon (12..32).
/// Defaults no codigo; config `Avalon.Class.<id>.Spells` sobrescreve por classe.
class ModAvalonClassSpells : public PlayerScript
{
public:
    ModAvalonClassSpells() :
        PlayerScript("ModAvalonClassSpells", { PLAYERHOOK_ON_LOGIN })
    {
        _enabled = sConfigMgr->GetOption<bool>("Avalon.ClassSpells.Enable", true);
        _defaults = {
            {12, {6673, 100, 78}},              // Barbarian  <- Warrior
            {13, {133, 116, 686, 1454, 172, 90001, 90002}}, // Bloodmage + Veinburst/Hemostasis
            {14, {133, 116, 1459}},             // Chronomancer
            {15, {686, 172, 689}},              // Cultist
            {16, {686, 348, 702}},              // Felsworn
            {17, {6673, 78, 7386}},             // Guardian
            {18, {20154, 635, 498}},            // Knight of Xoroth
            {19, {686, 172, 689, 688}},         // Necromancer
            {20, {5176, 8921, 5185}},           // Primalist
            {21, {133, 348, 2120}},             // Pyromancer
            {22, {2973, 3044, 1978}},           // Ranger
            {23, {1752, 2098, 1784}},           // Reaper
            {24, {686, 1459, 8092}},            // Runemaster
            {25, {8921, 5176, 8936}},           // Starcaller
            {26, {403, 331, 8042}},             // Stormbringer
            {27, {585, 2050, 139}},             // Sun Cleric
            {28, {20154, 635, 498}},            // Templar
            {29, {133, 116, 1459}},             // Tinker
            {30, {172, 3043, 1978}},            // Venomancer
            {31, {403, 8042, 331}},             // Witch Doctor
            {32, {2973, 3044, 1978}},           // Witch Hunter
        };
        LOG_INFO("module", "ModAvalonClassSpells {} ({} classes)", _enabled ? "enabled" : "disabled", _defaults.size());
    }

    void OnPlayerLogin(Player* player) override
    {
        if (!_enabled || !player || player->getClass() < 12)
            return;

        uint32 const classId = player->getClass();
        std::vector<uint32> spells = SpellsFor(classId);
        uint32 learned = 0;
        for (uint32 spellId : spells)
        {
            if (player->HasSpell(spellId))
                continue;
            player->learnSpell(spellId, false);
            if (player->HasSpell(spellId))
                ++learned;
            else
                LOG_ERROR("module", "classe {}: spell {} nao existe no servidor", classId, spellId);
        }
        if (learned)
        {
            // Persiste na hora: se o cliente cair sem logout, o personagem
            // ficaria sem os spells no proximo login.
            player->SaveToDB(false, false);
            LOG_INFO("module", "classe {} ({}): {} spells iniciais (salvo)", classId, player->GetName(), learned);
        }
    }

private:
    std::vector<uint32> SpellsFor(uint32 classId) const
    {
        std::string const key = "Avalon.Class." + std::to_string(classId) + ".Spells";
        std::string const configured = sConfigMgr->GetOption<std::string>(key, "");
        if (!configured.empty())
            return ParseSpells(configured);

        auto it = _defaults.find(classId);
        return it == _defaults.end() ? std::vector<uint32>{} : it->second;
    }

    static std::vector<uint32> ParseSpells(std::string const& csv)
    {
        std::vector<uint32> out;
        std::stringstream stream(csv);
        std::string token;
        while (std::getline(stream, token, ','))
        {
            try { out.push_back(static_cast<uint32>(std::stoul(token))); }
            catch (...) { }
        }
        return out;
    }

    bool _enabled = true;
    std::map<uint32, std::vector<uint32>> _defaults;
};

/// Mecanica v1 do Bloodmage: ao matar uma criatura, recupera % do HP maximo
/// ("sede de sangue"). Sem spell custom, sem mudanca no cliente.
class ModAvalonBloodmageMechanic : public PlayerScript
{
public:
    ModAvalonBloodmageMechanic() :
        PlayerScript("ModAvalonBloodmageMechanic", { PLAYERHOOK_ON_CREATURE_KILL })
    {
        _enabled = sConfigMgr->GetOption<bool>("Avalon.Bloodmage.HealOnKill.Enable", true);
        _percent = sConfigMgr->GetOption<uint32>("Avalon.Bloodmage.HealOnKill.Percent", 5);
        LOG_INFO("module", "ModAvalonBloodmageMechanic {} ({}% do HP por abate)",
            _enabled ? "enabled" : "disabled", _percent);
    }

    void OnPlayerCreatureKill(Player* killer, Creature* /*killed*/) override
    {
        if (!_enabled || !killer || killer->getClass() != 13 || _percent == 0)
            return;
        if (!killer->IsAlive())
            return;

        uint32 const heal = killer->GetMaxHealth() * _percent / 100;
        if (!heal)
            return;

        uint32 const before = killer->GetHealth();
        killer->ModifyHealth(int32(heal));
        uint32 const gained = killer->GetHealth() - before;
        if (!gained)
            return;

        // Cura no combat log nativo (SMSG_SPELLHEALLOG + Drain Life).
        WorldPacket log(SMSG_SPELLHEALLOG, 34);
        log << killer->GetPackGUID();
        log << killer->GetPackGUID();
        log << uint32(689);
        log << uint32(gained);
        log << uint32(heal - gained);
        log << uint32(0);
        log << uint8(0);
        log << uint8(0);
        killer->SendMessageToSet(&log, true);

        LOG_INFO("module", "Bloodmage {} recuperou {} HP por abate ({}% de {})",
            killer->GetName(), gained, _percent, killer->GetMaxHealth());
    }

private:
    bool _enabled = true;
    uint32 _percent = 5;
};

namespace
{
uint32 const SPELL_CRIMSON_DEBT  = 90000;
uint32 const SPELL_OVERFLOW      = 90010;
uint32 const SPELL_VITAL_RESERVE = 90022;
uint8  const CLASS_BLOODMAGE     = 13;

/// Spells do kit que cobram vida (ver docs/research/bloodmage-full-kit.md).
bool IsHealthCostSpell(uint32 spellId)
{
    switch (spellId)
    {
        case 90005: // Vein Lance
        case 90012: // Clotward
            return true;
        default:
            return false;
    }
}

uint8 CounterCap(uint32 spellId)
{
    SpellInfo const* info = sSpellMgr->GetSpellInfo(spellId);
    if (info && info->StackAmount)
        return uint8(info->StackAmount);
    return 10;
}

/// Sobe 1 stack de uma aura-contador e devolve o total depois do incremento.
uint8 GrantCounter(Player* player, uint32 spellId)
{
    uint8 const cap = CounterCap(spellId);
    Aura* aura = player->GetAura(spellId);
    if (!aura)
    {
        // AddAura ja aplica com 1 stack: contar como a primeira concessao.
        aura = player->AddAura(spellId, player);
        return aura ? aura->GetStackAmount() : 0;
    }

    // ModStackAmount recebe DELTA, nao valor absoluto (SpellAuras.cpp).
    if (aura->GetStackAmount() < cap)
        aura->ModStackAmount(1);

    return aura->GetStackAmount();
}
} // namespace

/// Recursos do Bloodmage: Crimson Debt (90000), Overflow (90010) e Vital Reserve (90022).
class ModAvalonBloodmageResource : public PlayerScript
{
public:
    ModAvalonBloodmageResource() :
        PlayerScript("ModAvalonBloodmageResource",
            { PLAYERHOOK_ON_SPELL_CAST, PLAYERHOOK_ON_CREATURE_KILL })
    {
        LOG_INFO("module", "ModAvalonBloodmageResource enabled (Crimson Debt {})",
            CounterCap(SPELL_CRIMSON_DEBT));
    }

    void OnPlayerSpellCast(Player* player, Spell* spell, bool skipCheck) override
    {
        if (!player || skipCheck || !spell || !spell->GetSpellInfo())
            return;
        if (player->getClass() != CLASS_BLOODMAGE)
            return;

        uint32 const spellId = spell->GetSpellInfo()->Id;
        if (!IsHealthCostSpell(spellId))
            return;

        GrantCrimsonDebt(player, spellId);

        uint8 const reserve = GrantCounter(player, SPELL_VITAL_RESERVE);
        LOG_INFO("module", "Bloodmage {} Vital Reserve {}", player->GetName(), reserve);
    }

    void OnPlayerCreatureKill(Player* killer, Creature* /*killed*/) override
    {
        if (!killer || killer->getClass() != CLASS_BLOODMAGE || !killer->IsAlive())
            return;

        GrantCrimsonDebt(killer, 0);
    }

private:
    void GrantCrimsonDebt(Player* player, uint32 sourceSpell)
    {
        uint8 const stacks = GrantCounter(player, SPELL_CRIMSON_DEBT);
        if (!stacks)
            return;

        LOG_INFO("module", "Bloodmage {} Crimson Debt {} (spell {})",
            player->GetName(), stacks, sourceSpell);

        if (stacks >= CounterCap(SPELL_CRIMSON_DEBT))
        {
            player->AddAura(SPELL_OVERFLOW, player);
            player->RemoveAura(SPELL_CRIMSON_DEBT);
            LOG_INFO("module", "Bloodmage {} Overflow aplicado (Crimson Debt cheio)",
                player->GetName());
        }
    }
};

/// Veinburst (90001): consome ate 3 stacks de Crimson Debt (aura 90000) para dano extra.
class spell_avalon_veinburst : public SpellScript
{
    PrepareSpellScript(spell_avalon_veinburst);

    void HandleHit(SpellEffIndex /*effIndex*/)
    {
        Player* caster = GetCaster()->ToPlayer();
        if (!caster || caster->getClass() != 13)
            return;

        Aura* shards = caster->GetAura(90000);
        if (!shards)
            return;

        uint8 const consumed = std::min<uint8>(shards->GetStackAmount(), 3);
        if (!consumed)
            return;

        shards->ModStackAmount(-int32(consumed));

        if (Unit* target = GetHitUnit())
            Unit::DealDamage(caster, target, 60u * consumed, nullptr, DIRECT_DAMAGE,
                SPELL_SCHOOL_MASK_SHADOW, GetSpellInfo(), false);

        LOG_INFO("module", "Veinburst de {} consumiu {} Crimson Debt", caster->GetName(), consumed);
    }

    void OnCastCheck()
    {
        Player* caster = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
        LOG_INFO("module", "Veinburst cast detectado ({})",
            caster ? caster->GetName() : "sem player");
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_avalon_veinburst::OnCastCheck);
        OnEffectHitTarget += SpellEffectFn(spell_avalon_veinburst::HandleHit, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

/// Hemostasis (90002): consome 3 Blood Shards para cura instantanea extra.
class spell_avalon_hemostasis : public SpellScript
{
    PrepareSpellScript(spell_avalon_hemostasis);

    void ConsumeShards()
    {
        Player* caster = GetCaster() ? GetCaster()->ToPlayer() : nullptr;
        if (!caster || caster->getClass() != 13)
            return;

        Aura* shards = caster->GetAura(90000);
        if (!shards || shards->GetStackAmount() < 3)
            return;

        shards->ModStackAmount(-3);

        uint32 const heal = 50u * 3u;
        uint32 const before = caster->GetHealth();
        caster->ModifyHealth(int32(heal));
        uint32 const gained = caster->GetHealth() - before;

        WorldPacket log(SMSG_SPELLHEALLOG, 34);
        log << caster->GetPackGUID();
        log << caster->GetPackGUID();
        log << uint32(90002);
        log << uint32(gained);
        log << uint32(heal - gained);
        log << uint32(0);
        log << uint8(0);
        log << uint8(0);
        caster->SendMessageToSet(&log, true);

        LOG_INFO("module", "Hemostasis de {} consumiu 3 Crimson Debt (+{} HP)", caster->GetName(), gained);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_avalon_hemostasis::ConsumeShards);
    }
};

void Addmod_custom_classesScripts()
{
    new ModAvalonClasses();
    new ModAvalonProbe();
    new ModAvalonClassSpells();
    new ModAvalonBloodmageMechanic();
    new ModAvalonBloodmageResource();
    RegisterSpellScript(spell_avalon_veinburst);
    RegisterSpellScript(spell_avalon_hemostasis);
}
