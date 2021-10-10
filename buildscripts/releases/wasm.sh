#!/usr/bin/env bash

set -e

cd "$(dirname "$0")"
cd ../..

meson \
  --cross-file platform/WebAssembly/wasm-cross-constants.txt \
  --cross-file platform/WebAssembly/wasm-cross.txt \
  --buildtype=release \
  --prefix=$(pwd)/build/DunGen.dist \
  --bindir='' \
  build

meson compile -C build

# reconfigure step is silly, but Meson is a bit over-opinionated and it's the
#   only way to get it to see the extra files emitted by Emscripten (the html and data)
meson \
  --cross-file platform/WebAssembly/wasm-cross-constants.txt \
  --cross-file platform/WebAssembly/wasm-cross.txt \
  --buildtype=release \
  --prefix=$(pwd)/build/DunGen.dist \
  --bindir='' \
  --reconfigure \
  build

cd build
meson install
