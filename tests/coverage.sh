#!/usr/bin/env bash
##
## EPITECH PROJECT, 2026
## zappy
## File description:
## Run the instrumented test binaries and print an llvm-cov coverage report
##
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT"

if command -v llvm-profdata >/dev/null 2>&1; then
  PROFDATA=(llvm-profdata)
  COV=(llvm-cov)
elif command -v xcrun >/dev/null 2>&1; then
  PROFDATA=(xcrun llvm-profdata)
  COV=(xcrun llvm-cov)
else
  echo "error: neither llvm-profdata nor xcrun found in PATH" >&2
  exit 1
fi

NET=./zappy_network_tests
RPC=./zappy_rpc_tests
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

LLVM_PROFILE_FILE="$WORK/net.profraw" "$NET" >/dev/null
LLVM_PROFILE_FILE="$WORK/rpc.profraw" "$RPC" >/dev/null

"${PROFDATA[@]}" merge -sparse \
  "$WORK/net.profraw" "$WORK/rpc.profraw" -o "$WORK/merged.profdata"

SOURCES=()
while IFS= read -r source_file; do
  SOURCES+=("$source_file")
done < <(find common/Network common/Rpc -name '*.hpp' -o -name '*.cpp')

"${COV[@]}" report "$NET" -object "$RPC" \
  -instr-profile="$WORK/merged.profdata" "${SOURCES[@]}"
