#!/usr/bin/env bash

set -e

cd "$(dirname "$0")"
cd ../..

meson --buildtype=release --prefix=$(pwd)/build/Dungen.dist/DunGen.app --bindir=Contents/MacOS build
meson compile -C build
cd build
meson install
codesign \
  --sign CFJYV723M9 \
  --entitlements ../platform/macOS/entitlements.plist \
  --options runtime \
  --timestamp \
  DunGen.app
# TODO: notarize
