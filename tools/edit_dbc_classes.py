#!/usr/bin/env python3
"""
Editor de DBC nativo (Linux) para adicionar classes customizadas ao ChrClasses.dbc.
Evita a necessidade de WDBX Editor via Wine.

Uso: executar a partir da raiz do wow-core (o caminho do DBC é relativo).
"""

import sys
from pathlib import Path

from dbc_lib import add_string, backup, read_dbc, write_dbc

DBC_PATH = Path("env/dist/dbc/ChrClasses.dbc")

# Classes Avalon (IDs 12-32, conforme Fase 2 do plano)
AVALON_CLASSES = [
    (12, "Barbarian", "BARBARIAN"),
    (13, "Bloodmage", "BLOODMAGE"),
    (14, "Chronomancer", "CHRONOMANCER"),
    (15, "Cultist", "CULTIST"),
    (16, "Felsworn", "FELSWORN"),
    (17, "Guardian", "GUARDIAN"),
    (18, "Knight of Xoroth", "KNIGHT_OF_XOROTH"),
    (19, "Necromancer", "NECROMANCER"),
    (20, "Primalist", "PRIMALIST"),
    (21, "Pyromancer", "PYROMANCER"),
    (22, "Ranger", "RANGER"),
    (23, "Reaper", "REAPER"),
    (24, "Runemaster", "RUNEMASTER"),
    (25, "Starcaller", "STARCALLER"),
    (26, "Stormbringer", "STORMBRINGER"),
    (27, "Sun Cleric", "SUN_CLERIC"),
    (28, "Templar", "TEMPLAR"),
    (29, "Tinker", "TINKER"),
    (30, "Venomancer", "VENOMANCER"),
    (31, "Witch Doctor", "WITCH_DOCTOR"),
    (32, "Witch Hunter", "WITCH_HUNTER"),
]

# Layout dos campos dos registros (baseado nos registros originais):
#   [0] ID da classe
#   [1] offset do nome (normal)
#   [2] unknown (1 nas classes jogáveis)
#   [3] unknown (1 nas classes jogáveis)
#   [4] offset do nome maiúsculo
#   [5] unknown (0)
DEFAULT_FIELDS = [0, 0, 1, 1, 0, 0] + [0] * 54  # 60 campos no total


def main() -> None:
    if not DBC_PATH.exists():
        print(f"ERRO: {DBC_PATH} não encontrado")
        sys.exit(1)

    print(f"Backup criado: {backup(DBC_PATH)}")

    dbc = read_dbc(DBC_PATH)
    print(f"DBC lido: {len(dbc.records)} registros, {dbc.field_count} campos, "
          f"string_block={len(dbc.string_block)} bytes")

    existing_ids = {dbc.int32(record)[0] for record in dbc.records}
    added = 0
    for class_id, name, name_upper in AVALON_CLASSES:
        if class_id in existing_ids:
            print(f"AVISO: ID {class_id} já existe, pulando...")
            continue

        fields = list(DEFAULT_FIELDS)
        fields[0] = class_id
        fields[1] = add_string(dbc.string_block, name)
        fields[4] = add_string(dbc.string_block, name_upper)

        dbc.records.append(dbc.pack_int32(fields))
        added += 1
        print(f"  Adicionada classe ID={class_id}: {name} ({name_upper})")

    dbc.records.sort(key=lambda record: dbc.int32(record)[0])

    write_dbc(dbc)
    print(f"\nConcluído: {len(dbc.records)} registros totais ({added} novas)")
    print(f"Arquivo modificado: {DBC_PATH}")


if __name__ == "__main__":
    main()
