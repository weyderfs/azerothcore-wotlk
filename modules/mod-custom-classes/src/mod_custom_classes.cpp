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
#include "Log.h"

#include "mod_custom_classes.h"

class ModCustomClasses : public PlayerScript
{
public:
    ModCustomClasses() :
        PlayerScript("ModCustomClasses",
        {
            PLAYERHOOK_ON_LOGIN,
        })
    {
    }

    void OnPlayerLogin(Player* player) override
    {
        HandleCustomClass(player);
    }

private:
    /// Verifica se o jogador tem a classe customizada e aplica passivas/inicialização
    void HandleCustomClass(Player* player)
    {
        if (!player || player->getClass() != CUSTOM_CLASS_ID)
            return;

        LOG_INFO("mod.custom.classes", "Player {} ({}) has custom class ID {}, applying initialization",
            player->GetName(), player->GetGUIDLow(), CUSTOM_CLASS_ID);

        // Aqui serão aplicadas as passivas, equipamento inicial e atributos da classe customizada
        // quando a Fase 2 (DBCs) e a injeção de SQL estiverem concluídas
    }
};

void AddSC_mod_custom_classes()
{
    new ModCustomClasses();
}
