#!/usr/bin/env bash

set -e

cd "$(dirname "$0")"
cd ../..
rm -rf build

CC=clang meson -Dc_std=gnu11 build
cd build
ninja
