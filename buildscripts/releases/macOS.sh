#!/usr/bin/env bash

set -e

cd "$(dirname "$0")"
cd ../..

meson --buildtype=release --prefix=$(pwd)/build/DunGen.dist/DunGen.app --bindir=Contents/MacOS build
meson compile -C build
cd build
meson install
cd DunGen.dist
codesign \
  --sign CFJYV723M9 \
  --entitlements ../../platform/macOS/entitlements.plist \
  --options runtime \
  --timestamp \
  DunGen.app
# TODO: notarize
