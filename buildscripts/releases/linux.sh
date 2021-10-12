#!/usr/bin/env bash

set -e

cd "$(dirname "$0")"
cd ../..

rm -rf build

CC=clang meson --buildtype=release --prefix=$(pwd)/build/DunGen_dist --bindir='' build
cd build
ninja
meson install
cd ..

cp docs/acknowledgements.md build/DunGen_dist