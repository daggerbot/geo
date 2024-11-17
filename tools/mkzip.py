# Copyright (c) 2024 Martin Mills <daggerbot@gmail.com>
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at https://mozilla.org/MPL/2.0/.

import argparse
from dataclasses import dataclass
import sys
from typing import Optional
import zipfile_zstd as zipfile

__all__ = []

@dataclass
class InputItem:
    path: str
    internal_name: str
    compression_method: int = zipfile.ZIP_STORED
    compression_level: Optional[int] = None

def parse_input(string: str) -> InputItem:
    """
    Parses an input argument with the following format: `path,internal_name[,compression_method[,compression_level]]`

    - `path` (required): The input file path.
    - `internal_name` (required): The name of the file as added to the ZIP archive.
    - `compression_method` (optional): Compression method: `none` (default), `deflate`, or `zstd`.
    - `compression_level` (optional): Algorithm-dependent compression level.
    """

    strings = string.split(',')
    path = strings[0]

    if path == '':
        raise ValueError(f"Missing path in input item: {string}")
    elif len(strings) < 2:
        raise ValueError(f"Missing internal name in input item: {string}")
    elif len(strings) > 4:
        raise ValueError(f"Too many parameters in input item: {string}")

    internal_name = strings[1]
    compression_method = zipfile.ZIP_STORED
    compression_level = -1

    if internal_name == '':
        raise ValueError(f"Missing internal name in input item: {string}")

    if len(strings) >= 3:
        match strings[2]:
            case '' | 'none':
                pass
            case 'deflate':
                compression_method = zipfile.ZIP_DEFLATED
            case 'zstd':
                compression_method = zipfile.ZIP_ZSTANDARD
            case _:
                raise ValueError(f"Invalid compression method in input item: {string}")

    if len(strings) >= 4 and len(strings[3]) > 0:
        compression_level = int(strings[3])

    return InputItem(
        path=path,
        internal_name=internal_name,
        compression_method=compression_method,
        compression_level=compression_level)

def parse_args():
    p = argparse.ArgumentParser(prog=sys.argv[0], description="tool for creating zip archives")
    p.add_argument('inputs', nargs='+', type=parse_input, help="input items with the format: path,internal_name[,compression_method[,compression_level]]")
    p.add_argument('-o', '--output', required=True, help="path to the zip file to create", metavar='PATH')
    p.add_argument('--min-comp-size', nargs=1, type=int, help="do not compress files smaller than this", metavar='NBYTES')
    return p.parse_args()

def main():
    args = parse_args()

    with zipfile.ZipFile(args.output, 'w') as outfile:
        for item in args.inputs:
            with open(item.path, 'rb') as infile:
                data = infile.read()

                # Do not compress if the file is very small (--min-comp-size NBYTES)
                if args.min_comp_size and len(data) < args.min_comp_size[0]:
                    item.compression_method = zipfile.ZIP_STORED
                    item.compression_level = None

                outfile.writestr(item.internal_name, data, item.compression_method, item.compression_level)

if __name__ == '__main__':
    main()
