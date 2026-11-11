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
        _enabled = sConfigMgr->GetOption<bool>("Avalon.Probe.Enable", true);
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

void Addmod_custom_classesScripts()
{
    new ModAvalonClasses();
    new ModAvalonProbe();
}
