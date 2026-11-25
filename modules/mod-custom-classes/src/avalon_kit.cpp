// Comandos de diagnostico do kit Avalon (`.avalon probe|cast|set`).
//
// Os `.cast`/`.set` do core sao Console::No: o SOAP nao tem sessao de jogador e
// nao consegue usa-los. Estes resolvem o jogador por nome, entao a prova em jogo
// do kit (tools/kit_soap.py) roda por console. O probe imprime estado em linhas
// chave=valor — contrato lido pelo driver.

#include "Chat.h"
#include "Creature.h"
#include "ObjectAccessor.h"
#include "Player.h"
#include "SpellAuraEffects.h"
#include "SpellAuras.h"

#include "avalon_kit.h"

#include <algorithm>
#include <string>

namespace
{
/// Teto do Rage no core (GetCreatePowers(POWER_RAGE) devolve 1000).
uint32 const RAGE_MAX = 1000;

float ShadowPower(Unit* unit)
{
    return unit->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_SHADOW);
}

void PrintAuras(ChatHandler* handler, Unit* unit)
{
    for (auto const& entry : unit->GetOwnedAuras())
    {
        Aura* aura = entry.second;
        if (!aura)
            continue;

        AuraEffect* effect = aura->GetEffect(0);
        handler->PSendSysMessage("AvalonKit: aura={} stacks={} amount={}",
            aura->GetId(), uint32(aura->GetStackAmount()),
            effect ? uint32(effect->GetAmount()) : 0u);
    }
}
} // namespace

Player* AvalonResolvePlayer(ChatHandler* handler, std::string const& name)
{
    if (Player* self = handler->GetPlayer())
    {
        if (name.empty() || self->GetName() == name)
            return self;
    }
    return ObjectAccessor::FindPlayerByName(name);
}

bool HandleKitProbe(ChatHandler* handler, std::string const& playerName,
    Optional<uint32> targetEntry)
{
    Player* player = AvalonResolvePlayer(handler, playerName);
    if (!player)
    {
        handler->SendSysMessage("Avalon: jogador nao encontrado (precisa estar online).");
        return false;
    }

    handler->PSendSysMessage(
        "AvalonKit: player={} level={} hp={}/{} rage={}/{} mana={}/{} sp={} cast={}",
        player->GetName(), uint32(player->GetLevel()),
        player->GetHealth(), player->GetMaxHealth(),
        player->GetPower(POWER_RAGE), player->GetMaxPower(POWER_RAGE),
        player->GetPower(POWER_MANA), player->GetMaxPower(POWER_MANA),
        uint32(ShadowPower(player)),
        player->IsNonMeleeSpellCast(false) ? 1u : 0u);

    PrintAuras(handler, player);

    if (targetEntry && *targetEntry)
    {
        Creature* target = player->FindNearestCreature(*targetEntry, 30.0f, false);
        if (!target)
        {
            handler->PSendSysMessage("AvalonKit: criatura {} nao encontrada em 30 yd.",
                *targetEntry);
            return false;
        }

        handler->PSendSysMessage("AvalonKit: target={} entry={} hp={}/{}",
            target->GetName(), target->GetEntry(), target->GetHealth(),
            target->GetMaxHealth());
        PrintAuras(handler, target);
    }

    return true;
}

bool HandleKitCast(ChatHandler* handler, std::string const& playerName,
    uint32 spellId, Optional<uint32> creatureEntry)
{
    Player* player = AvalonResolvePlayer(handler, playerName);
    if (!player)
    {
        handler->SendSysMessage("Avalon: jogador nao encontrado (precisa estar online).");
        return false;
    }

    Unit* target = player;
    if (creatureEntry && *creatureEntry)
    {
        Creature* creature = player->FindNearestCreature(*creatureEntry, 30.0f, false);
        if (!creature)
        {
            handler->PSendSysMessage("AvalonKit: criatura {} nao encontrada em 30 yd.",
                *creatureEntry);
            return false;
        }
        target = creature;
    }

    SpellCastResult const result = player->CastSpell(target, spellId, false);
    handler->PSendSysMessage("AvalonKit: cast spell={} target={} result={}",
        spellId, target->GetName(), uint32(result));
    return result == SPELL_CAST_OK;
}

bool HandleKitSet(ChatHandler* handler, std::string const& playerName,
    uint32 hpPercent, uint32 ragePercent)
{
    Player* player = AvalonResolvePlayer(handler, playerName);
    if (!player)
    {
        handler->SendSysMessage("Avalon: jogador nao encontrado (precisa estar online).");
        return false;
    }

    uint32 const hp = std::max<uint32>(1,
        player->GetMaxHealth() * std::min<uint32>(hpPercent, 100) / 100);
    player->SetHealth(hp);
    player->SetPower(POWER_RAGE, RAGE_MAX * std::min<uint32>(ragePercent, 100) / 100);

    handler->PSendSysMessage("AvalonKit: set hp={}/{} rage={}/{}",
        player->GetHealth(), player->GetMaxHealth(),
        player->GetPower(POWER_RAGE), player->GetMaxPower(POWER_RAGE));
    return true;
}
