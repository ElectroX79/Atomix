#!/usr/bin/env bash
set -e
for script in "$(dirname "${BASH_SOURCE[0]}")"/../benchmark/*no_writing.sh; do
    echo "Running $script"
    bash "$script"
done
