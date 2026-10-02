#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0
#
# Copyright (C) 2026 Umer Uddin <umer.uddin@mentallysanemainliners.org> 
#

import argparse
import configparser
import struct
import sys
from pathlib import Path

IMG_MAGIC = 0x7A7A1BEF
MAX_ENTRIES = 4
MAX_NAME_LEN = 15
MAX_OUTPUT_SIZE = 0x3E0000
BLOCK_SIZE = 512

ENTRY_FMT = "16sQQQ"
HEADER_FMT = "<II" + ENTRY_FMT * MAX_ENTRIES
HEADER_SIZE = struct.calcsize(HEADER_FMT)
HEADER_BLOCKS = (HEADER_SIZE + BLOCK_SIZE - 1) // BLOCK_SIZE

U64_MAX = (1 << 64) - 1

def round_up_blocks(nbytes):
    return (nbytes + BLOCK_SIZE - 1) // BLOCK_SIZE

def load_payloads():
    cfg = configparser.ConfigParser(
        inline_comment_prefixes=(";", "#"),
        interpolation=None,
    )
    if not cfg.read("blobs/config.ini"):
        sys.exit(f"could not read config.ini")

    payloads = []

    for sect in cfg.sections():
        s = cfg[sect]

        for key in ("file", "load_address", "name"):
            if key not in s:
                sys.exit(f"[{sect}] is missing required key '{key}'")

        name = s.get("name", sect).strip('"').strip("'")
        name_bytes = name.encode("utf-8")

        if len(name_bytes) > MAX_NAME_LEN:
            sys.exit(f"[{sect}] name '{name}' too long, 15 chars max")

        try:
            load_addr = int(s["load_address"], 0)
        except ValueError:
            sys.exit(f"[{sect}] bad load_address '{s['load_address']}'")

        if not 0 <= load_addr <= U64_MAX:
            sys.exit(f"[{sect}] load_address out of range")

        path = "blobs/" + s["file"].strip('"').strip("'")

        try:
            data = open(path, "rb").read()
        except OSError as e:
            sys.exit(f"[{sect}] cannot read '{path}': {e.strerror}")

        payloads.append({"section": sect, "name": name_bytes, "load_addr": load_addr, "data": data,})

    return payloads, "spl-bootable.img"

def check_payloads(payloads):
    if not payloads:
        sys.exit("no payload sections found")

    if len(payloads) > MAX_ENTRIES:
        sys.exit(f"{len(payloads)} images exceeds the maximum allowed ({MAX_ENTRIES})")

    spans = sorted(
        (p["load_addr"], p["load_addr"] + len(p["data"]), p["section"])
        for p in payloads if p["data"]
    )
    for (a0, a1, an), (b0, b1, bn) in zip(spans, spans[1:]):
        if b0 < a1:
            sys.exit(f"[{an}] (0x{a0:x}-0x{a1:x}) overlaps "
                f"[{bn}] (0x{b0:x}-0x{b1:x}) in RAM")

def build(payloads):
    entries = []
    next_block = HEADER_BLOCKS

    for p in payloads:
        size = len(p["data"])
        entries.append((p["name"], p["load_addr"], size, next_block))
        next_block += round_up_blocks(size)

    flat = []
    for i in range(MAX_ENTRIES):
        flat.extend(entries[i] if i < len(entries) else (b"", 0, 0, 0))

    header = struct.pack(HEADER_FMT, IMG_MAGIC, len(entries), *flat)
    header = header.ljust(HEADER_BLOCKS * BLOCK_SIZE, b"\0")

    blob = bytearray(header)
    for p in payloads:
        blob += p["data"]
        blob += b"\0" * (round_up_blocks(len(p["data"])) * BLOCK_SIZE - len(p["data"]))

    return bytes(blob)

def main():
    ap = argparse.ArgumentParser(description="thirty firmware builder")
    args = ap.parse_args()

    payloads, out_path = load_payloads()

    check_payloads(payloads)
    blob = build(payloads)
    open(out_path, "wb").write(Path("spl.bin").read_bytes() + blob)

    if len(blob) + len(Path("spl.bin").read_bytes()) > MAX_OUTPUT_SIZE:
        print(f"Output image is too large to boot via SD card ({len(blob)} bytes), maximum allowed is {MAX_OUTPUT_SIZE} bytes, boot will work, but data loss will occur when flashed to SD!")

    print(f"Wrote {len(payloads)} image(s), image at 'spl-bootable.img'")

if __name__ == "__main__":
    main()
