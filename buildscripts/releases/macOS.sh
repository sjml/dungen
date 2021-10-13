#!/usr/bin/env bash

set -e

cd "$(dirname "$0")"
cd ../..

rm -rf build

meson --buildtype=release --prefix=$(pwd)/build/DunGen_dist/DunGen.app --bindir=Contents/MacOS build
meson compile -C build
cd build
meson install
cd DunGen_dist
# codesigning / notarization happens in the GitHub Action

cd ../..

cp docs/acknowledgements.md build/DunGen_dist
