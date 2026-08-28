#!/usr/bin/env bash
set -e
cd "$(dirname "${BASH_SOURCE[0]}")"/../..
cmake -B build -G Ninja
cmake --build build --target buffer_benchmark
./build/buffer_benchmark
