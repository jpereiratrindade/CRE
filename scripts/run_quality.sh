#!/usr/bin/env bash

set -euo pipefail

BUILD_DIR="${BUILD_DIR:-build}"
SANITIZE_BUILD_DIR="${SANITIZE_BUILD_DIR:-build-sanitize}"

cmake -S . -B "$BUILD_DIR" -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build "$BUILD_DIR"
ctest --test-dir "$BUILD_DIR" --output-on-failure

cmake -S . -B "$SANITIZE_BUILD_DIR" -G Ninja -Dcre_ENABLE_SANITIZERS=ON
cmake --build "$SANITIZE_BUILD_DIR"
ASAN_OPTIONS=detect_leaks=1:halt_on_error=1 UBSAN_OPTIONS=halt_on_error=1 \
    ctest --test-dir "$SANITIZE_BUILD_DIR" --output-on-failure

if command -v clang-format >/dev/null 2>&1; then
    find src include tests -type f \( -name '*.cpp' -o -name '*.hpp' \) -print0 | xargs -0 -r clang-format --dry-run --Werror
fi

if command -v clang-tidy >/dev/null 2>&1; then
    find src tests -type f -name '*.cpp' -print0 | xargs -0 -r -n 1 clang-tidy -p "$BUILD_DIR" --config-file=.clang-tidy --quiet
fi

if command -v lizard >/dev/null 2>&1; then
    lizard -l cpp -C 15 src include tests
else
    echo "[complexity-guard] SKIP: lizard nao encontrado"
fi

python3 scripts/check_include_cycles.py --roots include src

python3 scripts/validate_tool_contracts.py
python3 scripts/validate_governance_repo.py
