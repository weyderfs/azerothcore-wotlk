#!/usr/bin/env python3
"""
Editor de DBC nativo (Linux) para habilitar as classes Avalon em todas as raças
jogáveis no CharBaseInfo.dbc.

CharBaseInfo.dbc tem 2 campos uint8 por registro: (RaceID, ClassID). O client usa
essa tabela para montar as combinações raça/classe na tela de criação de
personagem. O conjunto de raças já presente no arquivo é exatamente o conjunto de
raças jogáveis do client, então ele é reutilizado em vez de ficar hardcoded.

Uso: executar a partir da raiz do wow-core (o caminho do DBC é relativo).
"""

import struct
import sys
from pathlib import Path

from dbc_lib import backup, read_dbc, write_dbc

DBC_PATH = Path("env/dist/dbc/CharBaseInfo.dbc")

# Primeira e última classe Avalon (bloco contíguo de IDs 12-32)
AVALON_FIRST_CLASS = 12
AVALON_LAST_CLASS = 32

RECORD_FORMAT = "<BB"


def unpack(record: bytes) -> tuple[int, int]:
    return struct.unpack(RECORD_FORMAT, record)


def main() -> None:
    if not DBC_PATH.exists():
        print(f"ERRO: {DBC_PATH} não encontrado")
        sys.exit(1)

    print(f"Backup criado: {backup(DBC_PATH)}")

    dbc = read_dbc(DBC_PATH)
    if dbc.record_size != struct.calcsize(RECORD_FORMAT):
        raise ValueError(f"Registro inesperado: {dbc.record_size} bytes "
                         f"(esperado {struct.calcsize(RECORD_FORMAT)})")

    existing = {unpack(record) for record in dbc.records}
    races = sorted({race for race, _ in existing})
    print(f"DBC lido: {len(dbc.records)} combinações, raças jogáveis: {races}")

    added = 0
    for race in races:
        for class_id in range(AVALON_FIRST_CLASS, AVALON_LAST_CLASS + 1):
            if (race, class_id) in existing:
                continue
            dbc.records.append(struct.pack(RECORD_FORMAT, race, class_id))
            existing.add((race, class_id))
            added += 1

    dbc.records.sort(key=unpack)

    write_dbc(dbc)
    print(f"\nConcluído: {len(dbc.records)} combinações totais ({added} novas)")
    print(f"Arquivo modificado: {DBC_PATH}")


if __name__ == "__main__":
    main()
