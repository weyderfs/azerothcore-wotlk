#!/usr/bin/env python3
"""
Adiciona uma raça jogável "reskin" ao client 3.3.5a.

Uma raça reskin reutiliza o modelo de uma raça jogável existente (ex.: High Elf
usa o modelo de Blood Elf) e clona os dados de customização, exibição e som da
raça-base para o novo RaceID. Assim a tela de criação, o barbeiro e o retrato
funcionam como numa raça original.

Requer o core com RaceMgr (commit 515aeca57), que calcula raças jogáveis a
partir de ChrRaces.dbc — nenhum patch em C++ é necessário.

Uso: executar a partir da raiz do wow-core (os caminhos dos DBCs são relativos).
"""

import struct
import sys
from pathlib import Path

from dbc_lib import add_string, backup, read_dbc, write_dbc

DBC_DIR = Path("env/dist/dbc")

# Máscaras de raça usadas nos DBCs (bit = 1 << (raceId - 1))
ALLIANCE_RACES = (1 << 0) | (1 << 2) | (1 << 3) | (1 << 6) | (1 << 10)  # 1101
BLOODELF_RACE = 1 << 9                                                  # 512

RACES = {
    "high_elf": {
        "new_id": 22,
        "base_id": 10,       # Blood Elf: modelo + customização
        "name": "High Elf",
        "alliance": 0,       # 0 = Aliança
        "team_id": 7,        # 7 = Aliança
        "faction_id": 1,     # PLAYER, Human
        # Combinações liberadas na criação. Só entram classes que têm linha em
        # playercreateinfo para a raça (SQL); caso contrário a criação falha.
        "classes": [1, 2, 3, 4, 5, 6, 8, 9],
    },
}


def race_bit(race_id: int) -> int:
    return 1 << (race_id - 1)


def load(name: str):
    return read_dbc(DBC_DIR / f"{name}.dbc")


def save(name: str, dbc) -> None:
    write_dbc(dbc)
    print(f"  {name}: {len(dbc.records)} registros")


def drop_race_rows(dbc, race_field: int, race_id: int) -> int:
    before = len(dbc.records)
    dbc.records = [r for r in dbc.records if dbc.int32(r)[race_field] != race_id]
    return before - len(dbc.records)


def next_id(dbc, id_field: int = 0) -> int:
    return max(dbc.int32(r)[id_field] for r in dbc.records) + 1


def clone_rows(dbc, match, transform, id_field=0):
    """Clona registros que casam com `match`, aplicando `transform`.

    Se `id_field` não for None, atribui IDs novos sequenciais.
    """
    next_value = next_id(dbc, id_field) if id_field is not None else None
    clones = []
    for record in list(dbc.records):
        fields = dbc.int32(record)
        if not match(fields):
            continue
        fields = transform(fields)
        if id_field is not None:
            fields[id_field] = next_value
            next_value += 1
        clones.append(dbc.pack_int32(fields))
    dbc.records.extend(clones)
    return len(clones)


def clone_simple(name, race_field, spec, id_field=0, sort_key=None):
    """Clona todas as linhas da raça-base trocando o RaceID."""
    dbc = load(name)
    drop_race_rows(dbc, race_field, spec["new_id"])
    count = clone_rows(dbc, lambda f: f[race_field] == spec["base_id"],
                       lambda f: set_field(f, race_field, spec["new_id"]),
                       id_field=id_field)
    if sort_key:
        dbc.records.sort(key=lambda r: sort_key(dbc.int32(r)))
    save(name, dbc)
    print(f"    +{count} linhas clonadas da raça {spec['base_id']}")


def set_field(fields, index, value):
    fields[index] = value
    return fields


def add_mask_bit(fields, index, bit, groups):
    if fields[index] & groups:
        fields[index] |= bit
    return fields


def clone_chr_races(spec):
    dbc = load("ChrRaces")
    drop_race_rows(dbc, 0, spec["new_id"])

    base = next(dbc.int32(r) for r in dbc.records if dbc.int32(r)[0] == spec["base_id"])
    fields = list(base)
    fields[0] = spec["new_id"]
    fields[1] = base[1] & ~0x1          # limpa CHRRACES_FLAGS_NOT_PLAYABLE
    fields[2] = spec["faction_id"]
    fields[7] = spec["team_id"]
    fields[13] = spec["alliance"]
    fields[14] = add_string(dbc.string_block, spec["name"])  # nome (enUS)

    dbc.records.append(dbc.pack_int32(fields))
    dbc.records.sort(key=lambda r: dbc.int32(r)[0])
    save("ChrRaces", dbc)


def add_char_base_info(spec):
    dbc = load("CharBaseInfo")
    race = spec["new_id"]
    dbc.records = [r for r in dbc.records if struct.unpack("<BB", r)[0] != race]
    existing = {struct.unpack("<BB", r) for r in dbc.records}
    added = 0
    for class_id in spec["classes"]:
        if (race, class_id) not in existing:
            dbc.records.append(struct.pack("<BB", race, class_id))
            existing.add((race, class_id))
            added += 1
    dbc.records.sort(key=lambda r: struct.unpack("<BB", r))
    save("CharBaseInfo", dbc)
    print(f"    {added} combinações raça/classe: {spec['classes']}")


def clone_creature_displays(spec):
    race = spec["new_id"]
    extras = load("CreatureDisplayInfoExtra")

    # IDs de extras da raça nova, para remover também os displays que os usam
    stale_extra_ids = {extras.int32(r)[0] for r in extras.records if extras.int32(r)[1] == race}
    drop_race_rows(extras, 1, race)

    displays = load("CreatureDisplayInfo")
    before = len(displays.records)
    displays.records = [r for r in displays.records if displays.int32(r)[3] not in stale_extra_ids]
    removed = before - len(displays.records)

    id_map = {}
    next_extra = next_id(extras)
    for record in list(extras.records):
        fields = extras.int32(record)
        if fields[1] != spec["base_id"]:
            continue
        id_map[fields[0]] = next_extra
        fields[0] = next_extra
        fields[1] = race
        next_extra += 1
        extras.records.append(extras.pack_int32(fields))
    extras.records.sort(key=lambda r: extras.int32(r)[0])
    save("CreatureDisplayInfoExtra", extras)

    next_display = next_id(displays)
    added = 0
    for record in list(displays.records):
        fields = displays.int32(record)
        if fields[3] not in id_map:
            continue
        fields[0] = next_display
        fields[3] = id_map[fields[3]]
        next_display += 1
        displays.records.append(displays.pack_int32(fields))
        added += 1
    displays.records.sort(key=lambda r: displays.int32(r)[0])
    save("CreatureDisplayInfo", displays)
    print(f"    removidos {removed} displays, +{len(id_map)} extras, +{added} displays")


def clone_start_outfit(spec):
    dbc = load("CharStartOutfit")
    race = spec["new_id"]

    def is_base(fields):
        return fields[1] & 0xFF == spec["base_id"]

    def transform(fields):
        packed = fields[1]
        klass = (packed >> 8) & 0xFF
        gender = (packed >> 16) & 0xFF
        outfit = (packed >> 24) & 0xFF
        fields[1] = race | (klass << 8) | (gender << 16) | (outfit << 24)
        return fields

    # remove clones antigos (race no byte baixo do campo 1)
    before = len(dbc.records)
    dbc.records = [r for r in dbc.records if dbc.int32(r)[1] & 0xFF != race]
    removed = before - len(dbc.records)
    count = clone_rows(dbc, is_base, transform, id_field=0)
    dbc.records.sort(key=lambda r: dbc.int32(r)[0])
    save("CharStartOutfit", dbc)
    print(f"    removidas {removed}, +{count} outfits")


def apply_race_masks(spec):
    bit = race_bit(spec["new_id"])

    faction = load("Faction")
    changed = 0
    for i, record in enumerate(faction.records):
        fields = faction.int32(record)
        before = list(fields[2:6])
        for index in range(2, 6):
            add_mask_bit(fields, index, bit, ALLIANCE_RACES)
        if fields[2:6] != before:
            faction.records[i] = faction.pack_int32(fields)
            changed += 1
    save("Faction", faction)
    print(f"    {changed} facções com máscara atualizada")

    skills = load("SkillLineAbility")
    changed = 0
    for i, record in enumerate(skills.records):
        fields = skills.int32(record)
        if fields[3] & (ALLIANCE_RACES | BLOODELF_RACE):
            fields[3] |= bit
            skills.records[i] = skills.pack_int32(fields)
            changed += 1
    save("SkillLineAbility", skills)
    print(f"    {changed} habilidades com máscara atualizada")

    srci = load("SkillRaceClassInfo")
    changed = 0
    for i, record in enumerate(srci.records):
        fields = srci.int32(record)
        if fields[2] & (ALLIANCE_RACES | BLOODELF_RACE):
            fields[2] |= bit
            srci.records[i] = srci.pack_int32(fields)
            changed += 1
    save("SkillRaceClassInfo", srci)
    print(f"    {changed} perícias com máscara atualizada")

    tabs = load("TalentTab")
    changed = 0
    for i, record in enumerate(tabs.records):
        fields = tabs.int32(record)
        if fields[19] > 0:
            fields[19] |= bit
            tabs.records[i] = tabs.pack_int32(fields)
            changed += 1
    save("TalentTab", tabs)
    print(f"    {changed} abas de talento com máscara atualizada")


def main() -> None:
    if len(sys.argv) != 2 or sys.argv[1] not in RACES:
        print(f"Uso: {sys.argv[0]} <{'|'.join(RACES)}>")
        sys.exit(1)

    spec = RACES[sys.argv[1]]
    print(f"Raça {spec['new_id']} ({spec['name']}) a partir da raça {spec['base_id']}")

    for name in ("ChrRaces", "CharBaseInfo", "CharSections", "CharHairGeosets",
                 "CharHairTextures", "CharacterFacialHairStyles", "BarberShopStyle",
                 "CharStartOutfit", "CreatureDisplayInfo", "CreatureDisplayInfoExtra",
                 "NameGen", "VocalUISounds", "EmotesTextSound", "Faction",
                 "SkillLineAbility", "SkillRaceClassInfo", "TalentTab"):
        print(f"Backup: {backup(DBC_DIR / f'{name}.dbc')}")

    clone_chr_races(spec)
    add_char_base_info(spec)
    clone_simple("CharSections", 1, spec)
    clone_simple("CharHairGeosets", 1, spec)
    clone_simple("CharHairTextures", 1, spec)
    clone_simple("CharacterFacialHairStyles", 0, spec, id_field=None,
                 sort_key=lambda f: (f[0], f[1], f[2]))
    clone_simple("BarberShopStyle", 37, spec)
    clone_simple("NameGen", 2, spec)
    clone_simple("VocalUISounds", 2, spec)
    clone_simple("EmotesTextSound", 2, spec)
    clone_start_outfit(spec)
    clone_creature_displays(spec)
    apply_race_masks(spec)

    print("Concluído.")


if __name__ == "__main__":
    main()
