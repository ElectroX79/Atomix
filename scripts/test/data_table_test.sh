#!/usr/bin/env bash
set -e
cd "$(dirname "${BASH_SOURCE[0]}")"/../..
cmake -B build
cmake --build build --target data_table_test
./build/data_table_test
