#!/usr/bin/env bash

cd "$(dirname "$0")"

SHDC=../../sokol-tools-bin/bin/osx/sokol-shdc
SHADER_DIR=../Resources/shaders

formats=(glsl330 metal_macos)
fs=${formats[@]}
fs=${fs// /:}

for s in $(ls $SHADER_DIR/base/*.glsl); do
  for f in ${formats[@]}; do
    out=$f
    if [[ $f = "metal_macos" ]]; then
      out="metal"
    fi
    mkdir -p $SHADER_DIR/$out
    $SHDC -i $s -l $f -f bare -o $SHADER_DIR/$out/
  done
  mkdir -p $SHADER_DIR/headers
  $SHDC -i $s -l $fs -f sokol -o $SHADER_DIR/headers/$(basename $s).h
  # $SHDC -i $s -l $fs --bytecode -f sokol -o $SHADER_DIR/headers/$(basename $s).bytecode.h
  # rm -rf $SHADER_DIR/headers/*.{air,dia,metal,metallib}
done
