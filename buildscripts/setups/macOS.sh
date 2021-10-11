#!/usr/bin/env bash

set -e

cd "$(dirname "$0")"
cd ../..

meson build
meson compile -C build
