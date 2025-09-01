#!/bin/bash

set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$ROOT_DIR"

echo "==> Configuring CMake"
cmake -S . -B build

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

echo "==> Building PacManGame"
cmake --build build -j 8

echo "==> Running PacManGame"
exec ./build/PacManGame



