#!/usr/bin/env bash

set -e

cd "$(dirname "$0")"
cd ../..
rm -rf build

meson build
meson compile -C build
