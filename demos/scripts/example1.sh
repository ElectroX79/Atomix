#!/bin/bash

cmake -B build -G Ninja -S ../..
cmake --build build --target example1
./build/example1
