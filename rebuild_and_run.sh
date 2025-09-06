#!/bin/bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT_DIR"

mode="${1:-}"
shift || true || true

CONFIG_ARGS=(-S . -B build)

# If running net modes, disable singleplayer target to speed up
if [[ "$mode" == "localhost" || "$mode" == "server" || "$mode" == "client" ]]; then
  CONFIG_ARGS+=(-DENABLE_SINGLEPLAYER=OFF)
fi

echo "==> Configuring CMake"
cmake "${CONFIG_ARGS[@]}"

if command -v clang-format >/dev/null 2>&1; then
    echo "==> Running clang-format (in-place)"
    cmake --build build --target format
    echo "==> Verifying formatting (format-check)"
    cmake --build build --target format-check
else
    echo "WARNING: clang-format not found; skipping format steps" >&2
fi

if command -v clang-tidy >/dev/null 2>&1; then
    echo "==> Running clang-tidy"
    cmake --build build --target tidy
else
    echo "WARNING: clang-tidy not found; skipping lint step" >&2
fi

echo "==> Building all targets"
cmake --build build -j 8

if [[ -z "${mode}" ]]; then
  echo "Usage: $0 localhost | server [--port <num> --tick <hz>] | client [--ip <addr> --port <num>]" >&2
  exit 0
fi

case "$mode" in
  localhost)
    echo "==> Running server (localhost, 54000, 60 Hz)"
    ./build/pacman_server --port 54000 --tick 60 >/tmp/pacman_server.log 2>&1 &
    sleep 0.2
    echo "==> Running client"
    exec ./build/pacman_client --ip 127.0.0.1 --port 54000
    ;;
  server)
    echo "==> Running server with args: $*"
    exec ./build/pacman_server "$@"
    ;;
  client)
    echo "==> Running client with args: $*"
    exec ./build/pacman_client "$@"
    ;;
  *)
    echo "Unknown mode: $mode" >&2
    exit 1
    ;;
 esac



