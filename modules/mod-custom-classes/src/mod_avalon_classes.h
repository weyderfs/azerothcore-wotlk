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

#ifndef MOD_AVALON_CLASSES_H
#define MOD_AVALON_CLASSES_H

#include <set>
#include <unordered_map>

class Player;

/// Primeira classe Avalon (ID 12, conforme Fase 2 do plano)
#define AVALON_CLASS_ID CLASS_AVALON_BARBARIAN

/// Nome interno da classe Avalon
#define AVALON_CLASS_NAME "AvalonClass"

/// Opcode de probe do protocolo Avalon (fora do range stock 0x0000-0x051E).
/// Usado para provar o caminho servidor -> avalon-ext.dll (gate, prova 3).
#define AVALON_SMSG_PROBE 0x0A00

/// Spec ativa do personagem (tabela `avalon_spec`, characters DB).
/// 0 = nenhuma. Definida em `avalon_talents.cpp`.
uint32 ChosenSpec(uint64 guid);

/// Passivas que aquele personagem deve ter pelos nos de talento **da spec
/// ativa** com rank > 0. Le' os ranks do character DB e usa o catalogo ja'
/// carregado em memoria. Definida em `avalon_talents.cpp`.
std::set<uint32> TalentSpellsForGuid(uint64 guid);

/// Toda passiva declarada pelo catalogo de talentos. O reconcile so' remove
/// spells deste conjunto (mais o kit de classe), nunca spell fora dele.
/// Definida em `avalon_talents.cpp`.
std::set<uint32> AllTalentSpells();

/// Reconcilia o livro de spells com o catalogo do modulo: concede o que a
/// spec/nivel e os nos com rank permitem e remove o que deixou de valer.
/// Le' spec e ranks do banco. Definida em `bloodmage_levels.cpp`.
void ReconcileSpells(Player* player);

/// Reconcilia com estado explicito: usa `specId` e `rankedTalents` como dados,
/// sem reler o banco. Usado logo apos gravar spec ou limpar ranks, cujas
/// escritas assincronas ainda podem nao ter chegado.
void ReconcileSpells(Player* player, uint32 specId, std::set<uint32> const& rankedTalents);

/// Carrega `avalon_class_spells` da classe 13 para memoria. Uma vez, no boot:
/// o catalogo nao muda em runtime e o reconcile roda no login, na troca de spec
/// e a cada nivel. Definida em `bloodmage_levels.cpp`.
void LoadClassSpellCatalogue();

/// spec_id do kit a que um spell real pertence; 0 = base da classe.
/// Vazio enquanto o catalogo nao for carregado. Definida em `bloodmage_levels.cpp`.
std::unordered_map<uint32, uint32> const& ClassSpellSpecs();

#endif
