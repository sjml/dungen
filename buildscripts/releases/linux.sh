#!/usr/bin/env bash

set -e

cd "$(dirname "$0")"
cd ../..

rm -rf build

CC=clang meson --buildtype=release --prefix=$(pwd)/build/DunGen_dist/AppDir --bindir='' build
cd build
ninja
meson install
wget "https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage"
chmod +x appimagetool-x86_64.AppImage
./appimagetool-x86_64.AppImage ./DunGen_dist/AppDir ./DunGen_dist/DunGen.AppImage
rm -rf ./DunGen_dist/AppDir
cd ..

cp docs/acknowledgements.md build/DunGen_dist