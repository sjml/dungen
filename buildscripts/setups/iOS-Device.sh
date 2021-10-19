#!/usr/bin/env bash

set -e

cd "$(dirname "$0")"
cd ../..

meson \
  --prefix=$(pwd)/build/DunGen_dist/DunGen.app \
  --bindir='' \
  --cross-file platform/iOS/ios-device-cross.txt \
  --cross-file platform/iOS/ios-base-cross.txt \
  build

meson compile -C build

cd build
meson install
