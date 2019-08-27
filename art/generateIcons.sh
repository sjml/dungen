#!/bin/bash

INPUT="AppLogo.png"
BASE=${INPUT%.*}

# Mac
MAC_SIZES=(1024 512 256 128 64 32 16)
MAC_OUTPUT_DIR="mac_appiconset"
rm -rf $MAC_OUTPUT_DIR
mkdir -p $MAC_OUTPUT_DIR
for s in ${MAC_SIZES[@]}; do
  convert $INPUT -resize $sx$s $MAC_OUTPUT_DIR/$s.png
done

# Windows
WIN_SIZES=(256 128 96 64 48 32 16)
sizeList=$(printf ",%s" "${WIN_SIZES[@]}")
sizeList=${sizeList:1}
convert $INPUT -resize 256 -define icon:auto-resize="$sizeList" $BASE.ico
