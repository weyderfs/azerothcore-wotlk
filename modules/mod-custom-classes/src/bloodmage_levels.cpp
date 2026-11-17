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

        // Concede spells da tabela avalon_class_spells (world DB) onde level <= playerLevel e nao conhecidas
        QueryResult result = WorldDatabase.Query(
            "SELECT spell, level FROM `avalon_class_spells` WHERE `class` = 13 AND `level` <= {}", playerLevel);

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

        // Reconcilia: garantir que tudo o que o level do personagem permitir
        // esteja aprendido (cobre o caso de login após restart sem save, etc.)
        QueryResult result = WorldDatabase.Query(
            "SELECT spell, level FROM `avalon_class_spells` WHERE `class` = 13 AND `level` <= {}", player->GetLevel());

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