#!/usr/bin/env bash

set -e

cd "$(dirname "$0")"
cd ../..
rm -rf build

CC=clang meson build
cd build
ninja
