#!/usr/bin/env bash

cd "$(dirname "$0")"

SHDC=../../sokol-tools-bin/bin/osx/sokol-shdc
SHADER_DIR=../Resources/shaders

# all formats: glsl330 glsl100 glsl300es hlsl4 hlsl5 metal_macos metal_ios metal_sim wgpu
declare -A formats
formats[glsl330]=glsl
formats[glsl300es]=glsl
formats[metal_macos]=metallib
formats[metal_ios]=metallib
formats[metal_sim]=metal

fs=${!formats[@]}
fs=${fs// /:}

mkdir -p $SHADER_DIR/tmp
for s in $(ls $SHADER_DIR/base/*.glsl); do
  shadername=$(basename ${s%.*})

  # for f in ${!formats[@]}; do
  #   suffix=${formats[$f]}
  #   mkdir -p $SHADER_DIR/$f
  #   $SHDC --input $s \
  #     --slang $f \
  #     --format bare \
  #     --bytecode \
  #     --tmpdir $SHADER_DIR/tmp \
  #     --output $SHADER_DIR/$f/$shadername

  #   # outputting to different directories, so don't need shdc's overzealous names.
  #   #   this mv only works because the file name always matches the program name
  #   mv ${SHADER_DIR}/${f}/${shadername}_${shadername}_${f}_vs.${suffix} ${SHADER_DIR}/${f}/${shadername}_vs.${suffix}
  #   mv ${SHADER_DIR}/${f}/${shadername}_${shadername}_${f}_fs.${suffix} ${SHADER_DIR}/${f}/${shadername}_fs.${suffix}
  # done

  mkdir -p ../src/shaders
  $SHDC --input $s \
    --slang $fs \
    --format sokol \
    --bytecode \
    --ifdef \
    --tmpdir $SHADER_DIR/tmp \
    --output ../src/shaders/${shadername}_bytecode.h
  $SHDC --input $s \
    --slang $fs \
    --format sokol \
    --ifdef \
    --tmpdir $SHADER_DIR/tmp \
    --output ../src/shaders/${shadername}.h
done
rm -rf $SHADER_DIR/tmp
