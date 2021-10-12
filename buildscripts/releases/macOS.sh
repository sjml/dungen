#!/usr/bin/env bash

set -e

cd "$(dirname "$0")"
cd ../..

rm -rf build

meson --buildtype=release --prefix=$(pwd)/build/DunGen_dist/DunGen.app --bindir=Contents/MacOS build
if [[ $CI -eq "true" ]]; then
  echo "Running in CI"
  meson configure -Ddummy_backend=true build
fi
meson compile -C build
cd build
meson install
cd DunGen_dist
# codesign \
#   --sign CFJYV723M9 \
#   --entitlements ../../platform/macOS/entitlements.plist \
#   --options runtime \
#   --timestamp \
#   DunGen.app
# TODO: notarize
cd ../..

cp docs/acknowledgements.md build/DunGen_dist
