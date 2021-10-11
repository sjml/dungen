#!/usr/bin/env bash

set -e

cd "$(dirname "$0")"
cd ../..

CC=clang meson build
cd build
ninja
