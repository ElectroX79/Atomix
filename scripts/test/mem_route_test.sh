#!/usr/bin/env bash
set -e

cd "$(dirname "${BASH_SOURCE[0]}")"/../..
cmake -B build
cmake --build build --target mem_route_test
./build/mem_route_test
