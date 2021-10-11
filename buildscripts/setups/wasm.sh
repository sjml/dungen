#!/usr/bin/env bash

set -e

cd "$(dirname "$0")"
cd ../..

meson \
  --cross-file local/wasm-cross-constants.txt \
  --cross-file platform/WebAssembly/wasm-cross.txt \
  build

meson compile -C build
