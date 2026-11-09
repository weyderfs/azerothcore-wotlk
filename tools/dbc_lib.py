#!/usr/bin/env python3
"""
Leitura/escrita genérica de arquivos DBC (WDBC) do client 3.3.5a.

Formato:
  Header: magic(4) + record_count(4) + field_count(4) + record_size(4) + string_block_size(4)
  Registros: record_count x record_size bytes
  Bloco de strings: string_block_size bytes (strings null-terminated)

Os registros são mantidos como bytes crus, portanto o módulo serve para qualquer
DBC, independente da largura dos campos (int32, uint8, float...).
"""

import shutil
import struct
from dataclasses import dataclass
from pathlib import Path


@dataclass
class DBC:
    path: Path
    field_count: int
    record_size: int
    records: list[bytes]
    string_block: bytearray

    @property
    def field_width(self) -> int:
        """Número de campos int32 reais por registro.

        O header de alguns DBCs (ex.: CharStartOutfit) informa field_count
        inconsistente; record_size é a fonte de verdade.
        """
        return self.record_size // 4

    def int32(self, record: bytes) -> list[int]:
        """Interpreta um registro como int32 little-endian."""
        return list(struct.unpack(f"<{self.field_width}i", record))

    def pack_int32(self, values: list[int]) -> bytes:
        """Serializa valores int32 little-endian no tamanho de um registro."""
        return struct.pack(f"<{self.field_width}i", *values)


def read_dbc(path: Path) -> DBC:
    data = Path(path).read_bytes()
    if data[0:4] != b"WDBC":
        raise ValueError(f"Não é um arquivo DBC válido (magic={data[0:4]})")

    record_count, field_count, record_size, string_block_size = struct.unpack_from("<IIII", data, 4)
    records_start = 20
    strings_start = records_start + record_count * record_size

    records = [data[records_start + i * record_size:records_start + (i + 1) * record_size]
               for i in range(record_count)]
    string_block = bytearray(data[strings_start:strings_start + string_block_size])

    return DBC(Path(path), field_count, record_size, records, string_block)


def write_dbc(dbc: DBC) -> None:
    for record in dbc.records:
        if len(record) != dbc.record_size:
            raise ValueError(f"Registro com tamanho {len(record)}, esperado {dbc.record_size}")

    header = struct.pack("<4sIIII", b"WDBC", len(dbc.records), dbc.field_count,
                         dbc.record_size, len(dbc.string_block))
    with open(dbc.path, "wb") as f:
        f.write(header)
        f.write(b"".join(dbc.records))
        f.write(dbc.string_block)


def add_string(string_block: bytearray, s: str) -> int:
    """Anexa (ou reutiliza) uma string null-terminated e retorna seu offset."""
    encoded = s.encode("utf-8") + b"\x00"
    offset = string_block.find(encoded)
    while offset != -1 and offset != 0 and string_block[offset - 1] != 0:
        offset = string_block.find(encoded, offset + 1)
    if offset != -1:
        return offset
    offset = len(string_block)
    string_block.extend(encoded)
    return offset


def resolve_string(string_block: bytearray, offset: int) -> str:
    if not 0 <= offset < len(string_block):
        return ""
    end = string_block.find(b"\x00", offset)
    return string_block[offset:end if end != -1 else len(string_block)].decode("utf-8", "replace")


def backup(path: Path) -> Path:
    """Copia o arquivo para <nome>.dbc.bak (preserva o backup original)."""
    dest = Path(path).with_suffix(".dbc.bak")
    if not dest.exists():
        shutil.copy2(path, dest)
    return dest
