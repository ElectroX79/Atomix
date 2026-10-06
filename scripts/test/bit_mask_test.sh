#!/usr/bin/env bash
set -e
cd "$(dirname "${BASH_SOURCE[0]}")"/../..
cmake -B build -G Ninja
cmake --build build --target bit_mask_test
./build/bit_mask_test
