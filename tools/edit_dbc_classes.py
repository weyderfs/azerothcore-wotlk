#!/usr/bin/env python3
"""
Editor de DBC nativo (Linux) para adicionar classes customizadas ao ChrClasses.dbc.
Evita a necessidade de WDBX Editor via Wine.

Formato DBC (WDBC):
  Header: magic(4) + record_count(4) + field_count(4) + record_size(4) + string_block_size(4)
  Registros: record_count x record_size bytes (cada campo é int32 de 4 bytes)
  Bloco de strings: string_block_size bytes (strings null-terminated)
"""

import struct
import sys
import shutil
from pathlib import Path

DBC_PATH = Path("env/dist/dbc/ChrClasses.dbc")

# Classes Avalon a adicionar (IDs 12-32, conforme Fase 2 do plano)
# Nomes definidos para o servidor Avalon
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

# Valores padrão para os campos dos registros (baseado nos registros existentes)
# field[0]: ID da classe
# field[1]: offset de string para o nome (normal)
# field[2]: unknown (1 para a maioria das classes jogáveis)
# field[3]: unknown (1 para a maioria)
# field[4]: offset de string para o nome maiúsculo
# field[5]: unknown (0)
DEFAULT_FIELDS = [0, 0, 1, 1, 0, 0] + [0] * 54  # 60 campos no total


def read_dbc(path):
    """Lê um arquivo DBC e retorna header, registros e bloco de strings."""
    with open(path, "rb") as f:
        data = bytearray(f.read())

    magic = data[0:4]
    if magic != b"WDBC":
        raise ValueError(f"Não é um arquivo DBC válido (magic={magic})")

    record_count, field_count, record_size, string_block_size = struct.unpack_from("<IIII", data, 4)
    header_size = 20
    records_start = header_size
    strings_start = records_start + record_count * record_size

    records = []
    for i in range(record_count):
        offset = records_start + i * record_size
        rec = list(struct.unpack_from(f"<{field_count}i", data, offset))
        records.append(rec)

    string_block = data[strings_start:strings_start + string_block_size]

    return {
        "magic": magic,
        "record_count": record_count,
        "field_count": field_count,
        "record_size": record_size,
        "string_block_size": string_block_size,
        "records": records,
        "string_block": bytearray(string_block),
        "header_size": header_size,
        "strings_start": strings_start,
    }


def add_string(string_block, current_size, s):
    """Adiciona uma string null-terminada ao bloco de strings e retorna o offset."""
    encoded = s.encode("utf-8") + b"\x00"
    offset = current_size
    string_block[current_size:current_size + len(encoded)] = encoded
    return offset, current_size + len(encoded)


def write_dbc(path, dbc):
    """Escreve o DBC modificado de volta ao arquivo."""
    new_record_count = len(dbc["records"])
    new_string_block_size = len(dbc["string_block"])

    # Construir o novo arquivo
    header = struct.pack("<4sIIII",
                         dbc["magic"],
                         new_record_count,
                         dbc["field_count"],
                         dbc["record_size"],
                         new_string_block_size)

    records_data = bytearray()
    for rec in dbc["records"]:
        records_data.extend(struct.pack(f"<{dbc['field_count']}i", *rec))

    with open(path, "wb") as f:
        f.write(header)
        f.write(records_data)
        f.write(dbc["string_block"])


def main():
    dbc_path = Path(DBC_PATH)
    if not dbc_path.exists():
        print(f"ERRO: {dbc_path} não encontrado")
        sys.exit(1)

    # Backup
    backup = dbc_path.with_suffix(".dbc.bak")
    shutil.copy2(dbc_path, backup)
    print(f"Backup criado: {backup}")

    dbc = read_dbc(dbc_path)
    print(f"DBC lido: {dbc['record_count']} registros, {dbc['field_count']} campos, "
          f"string_block={dbc['string_block_size']} bytes")

    # Verificar se IDs já existem
    existing_ids = {r[0] for r in dbc["records"]}
    for class_id, name, name_upper in AVALON_CLASSES:
        if class_id in existing_ids:
            print(f"AVISO: ID {class_id} já existe, pulando...")
            continue

        # Adicionar strings ao bloco
        name_offset, new_size = add_string(dbc["string_block"], dbc["string_block_size"], name)
        dbc["string_block_size"] = new_size
        name_upper_offset, new_size = add_string(dbc["string_block"], dbc["string_block_size"], name_upper)
        dbc["string_block_size"] = new_size

        # Criar novo registro
        new_rec = list(DEFAULT_FIELDS)
        new_rec[0] = class_id      # ID da classe
        new_rec[1] = name_offset   # offset do nome normal
        new_rec[2] = 1             # unknown
        new_rec[3] = 1             # unknown
        new_rec[4] = name_upper_offset  # offset do nome maiúsculo
        new_rec[5] = 0             # unknown

        dbc["records"].append(new_rec)
        print(f"  Adicionada classe ID={class_id}: {name} ({name_upper})")

    # Ordenar registros por ID
    dbc["records"].sort(key=lambda r: r[0])

    write_dbc(dbc_path, dbc)
    print(f"\nConcluído: {len(dbc['records'])} registros totais ({dbc['record_count']} originais + {len(AVALON_CLASSES)} novas)")
    print(f"Arquivo modificado: {dbc_path}")


if __name__ == "__main__":
    main()
