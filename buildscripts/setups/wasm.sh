#!/usr/bin/env bash

set -e

cd "$(dirname "$0")"
cd ../..
rm -rf build

if [[ ! -f local/wasm-cross-constants.txt ]]; then
  echo "Need a \`local/wasm-cross-constants.txt\` file in this form:"
  echo "[constants]"
  echo "EMSDK_BASE = '/path/to/emsdk/upstream/emscripten'"
  exit 1
fi

EMSDK_VERSION=$(cat platform/WebAssembly/EMSDK_VERSION.txt)
EMSDK_PATH=$(cat local/wasm-cross-constants.txt | grep EMSDK_BASE | cut -d " " -f 3 | cut -d "'" -f 2)
$EMSDK_PATH/../../emsdk activate $EMSDK_VERSION

meson \
  --cross-file local/wasm-cross-constants.txt \
  --cross-file platform/WebAssembly/wasm-cross.txt \
  build

meson compile -C build
