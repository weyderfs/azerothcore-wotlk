/*
 * Sistema de trainer por spec do Avalon.
 *
 * O `trainer_spell` guarda o catalogo completo (wrappers 90201+); a lista que o
 * jogador ve' e' filtrada por spec: spells base da classe (spec_id = 0) sempre,
 * mais as spells da spec ativa. Trocar de spec troca a lista.
 *
 * Espelha o filtro que o cliente faz sozinho nao: o cliente descarta a spell se
 * o skillLine resolver para 0, mas nao sabe nada de spec. Ver
 * `.agents/plans/avalon-trainer-skillline/`.
 */

#include "Creature.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "NPCPackets.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "SpellInfo.h"
#include "SpellMgr.h"

#include <unordered_map>

#include "mod_avalon_classes.h"

namespace
{
/// Primeira classe Avalon (12 = Barbarian).
constexpr uint8 AVALON_FIRST_CLASS = 12;

/// Spell real ensinada por um wrapper (efeito LEARN_SPELL).
/// 0 quando a spell nao e' wrapper.
uint32 RealSpellOf(uint32 spellId)
{
    SpellInfo const* info = sSpellMgr->GetSpellInfo(spellId);
    if (!info)
        return 0;

    for (SpellEffectInfo const& effect : info->GetEffects())
        if (effect.IsEffect(SPELL_EFFECT_LEARN_SPELL) && effect.TriggerSpell)
            return effect.TriggerSpell;

    return 0;
}

/// spell -> spec_id do catalogo da classe (world DB). 0 = base.
std::unordered_map<uint32, uint32> ClassSpellSpecs(uint8 classId)
{
    std::unordered_map<uint32, uint32> specs;
    QueryResult result = WorldDatabase.Query(
        "SELECT `spell`, `spec_id` FROM `avalon_class_spells` WHERE `class` = {}", classId);
    if (!result)
        return specs;

    do
    {
        Field* fields = result->Fetch();
        specs[fields[0].Get<uint32>()] = fields[1].Get<uint32>();
    } while (result->NextRow());

    return specs;
}
} // namespace

class ModAvalonTrainerSpecFilter : public PlayerScript
{
public:
    ModAvalonTrainerSpecFilter() :
        PlayerScript("ModAvalonTrainerSpecFilter",
        {
            PLAYERHOOK_ON_BEFORE_RECEIVE_SPELL_LIST_FROM_TRAINER,
        })
    {
        LOG_INFO("module", "ModAvalonTrainerSpecFilter loaded — trainer list filtrada por spec");
    }

    void OnPlayerBeforeReceiveSpellListFromTrainer(Player* player, Creature* /*trainer*/,
        WorldPackets::NPC::TrainerList& trainerList) override
    {
        if (!player || player->getClass() < AVALON_FIRST_CLASS || trainerList.Spells.empty())
            return;

        uint8 const classId = player->getClass();
        auto const specs = ClassSpellSpecs(classId);
        if (specs.empty())
            return;

        uint32 const activeSpec = ChosenSpec(player->GetGUID().GetCounter());

        uint32 removed = 0;
        auto it = trainerList.Spells.begin();
        while (it != trainerList.Spells.end())
        {
            uint32 const real = RealSpellOf(uint32(it->SpellID));
            auto const found = specs.find(real);
            bool const otherSpec = found != specs.end()
                && found->second != 0
                && found->second != activeSpec;

            if (otherSpec)
            {
                it = trainerList.Spells.erase(it);
                ++removed;
            }
            else
            {
                ++it;
            }
        }

        if (removed)
            LOG_DEBUG("module",
                "classe {} spec {}: {} spells de outra spec removidas da lista do trainer",
                uint32(classId), activeSpec, removed);
    }
};

void AddAvalonTrainerScripts()
{
    new ModAvalonTrainerSpecFilter();
}
