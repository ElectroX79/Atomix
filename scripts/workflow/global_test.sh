#!/usr/bin/env bash
set -e
for script in "$(dirname "${BASH_SOURCE[0]}")"/../test/*.sh; do
    echo "Running $script"
    bash "$script"
done
