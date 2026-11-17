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
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Player.h"
#include "Config.h"
#include "Log.h"
#include "DatabaseEnv.h"
#include "ObjectGuid.h"

// Read spec active of the player from avalon_spec (characters DB)
static inline uint32 ChosenSpecFor(uint64 guid)
{
    auto result = CharacterDatabase.Query("SELECT `spec` FROM `avalon_spec` WHERE `guid` = {}", guid);
    return result ? result->Fetch()[0].Get<uint32>() : 0;
}

class ModAvalonBloodmageProgressao : public PlayerScript
{
public:
    ModAvalonBloodmageProgressao() :
        PlayerScript("ModAvalonBloodmageProgressao",
        {
            PLAYERHOOK_ON_LOGIN,
            PLAYERHOOK_ON_LEVEL_CHANGED,
        })
    {
        LOG_INFO("module", "ModAvalonBloodmageProgressao loaded — bloodmage level progression");
    }

    void OnPlayerLevelChanged(Player* player, uint8 /*oldLevel*/) override
    {
        if (!player || player->getClass() != 13)
            return;

        uint32 playerLevel = player->GetLevel();
        uint32 const specId = ChosenSpecFor(player->GetGUID().GetCounter());

        // Concede spells da tabela avalon_class_spells (world DB) onde level <= playerLevel,
        // para spec_id = 0 (base) ou spec_id == spec do personagem.
        QueryResult result = WorldDatabase.Query(
            "SELECT `spell`, `level` FROM `avalon_class_spells` "
            "WHERE `class` = 13 AND (`spec_id` = 0 OR `spec_id` = {}) AND `level` <= {}",
            specId, playerLevel);

        if (!result)
        {
            LOG_INFO("module", "ModAvalonBloodmageProgressao: no rows for class 13 at level {}", playerLevel);
            return;
        }

        int granted = 0;
        do
        {
            uint32 spellId = result->Fetch()[0].Get<uint32>();
            if (!player->HasSpell(spellId))
            {
                player->learnSpell(spellId, false);
                ++granted;
                LOG_INFO("module", "ModAvalonBloodmageProgressao: player {} learned spell {} at level {}", player->GetName(), spellId, playerLevel);
            }
        } while (result->NextRow());

        if (granted)
        {
            player->SaveToDB(false, false);
            LOG_INFO("module", "ModAvalonBloodmageProgressao: {} spells granted to {} at level {}", granted, player->GetName(), playerLevel);
        }
    }

    void OnPlayerLogin(Player* player) override
    {
        if (!player || player->getClass() != 13)
            return;

        uint32 playerLevel = player->GetLevel();
        uint32 const specId = ChosenSpecFor(player->GetGUID().GetCounter());

        // Reconcilia: garantir que tudo o que o level do personagem permitir
        // esteja aprendido (cobre o caso de login após restart sem save, etc.)
        QueryResult result = WorldDatabase.Query(
            "SELECT `spell`, `level` FROM `avalon_class_spells` "
            "WHERE `class` = 13 AND (`spec_id` = 0 OR `spec_id` = {}) AND `level` <= {}",
            specId, playerLevel);

        if (!result)
        {
            LOG_INFO("module", "ModAvalonBloodmageProgressao: no rows for class 13 on login");
            return;
        }

        int reconciled = 0;
        do
        {
            uint32 spellId = result->Fetch()[0].Get<uint32>();
            if (!player->HasSpell(spellId))
            {
                player->learnSpell(spellId, false);
                ++reconciled;
                LOG_INFO("module", "ModAvalonBloodmageProgressao: reconciled spell {} for {} at level {}", spellId, player->GetName(), player->GetLevel());
            }
        } while (result->NextRow());

        if (reconciled)
        {
            player->SaveToDB(false, false);
            LOG_INFO("module", "ModAvalonBloodmageProgressao: {} spells reconciled for {} at level {}", reconciled, player->GetName(), player->GetLevel());
        }
    }
};

void AddAvalonBloodmageProgressaoScripts()
{
    new ModAvalonBloodmageProgressao();
}