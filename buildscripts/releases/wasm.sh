#!/usr/bin/env bash

set -e

cd "$(dirname "$0")"
cd ../..
rm -rf build

if [[ ! -f local/wasm-cross-constants.txt ]]; then
  echo "Need a \`local/wasm-cross-constants.txt\` file in this form:"
  echo "[constants]"
  echo "EMSDK_BASE = '/absolute/path/to/emsdk/upstream/emscripten'"
  exit 1
fi

EMSDK_VERSION=$(cat platform/WebAssembly/EMSDK_VERSION.txt)
EMSDK_PATH=$(cat local/wasm-cross-constants.txt | grep EMSDK_BASE | cut -d " " -f 3 | cut -d "'" -f 2)
$EMSDK_PATH/../../emsdk activate $EMSDK_VERSION


meson \
  --cross-file local/wasm-cross-constants.txt \
  --cross-file platform/WebAssembly/wasm-cross.txt \
  --buildtype=release \
  --prefix=$(pwd)/build/DunGen_dist \
  --bindir='' \
  build

meson compile -C build

# The reconfigure step is silly, but Meson is a bit over-opinionated and it's the
#   only way to get it to see the extra files emitted by Emscripten (the html and data).
# Why not just copy them manually here? Why indeed.
meson \
  --cross-file local/wasm-cross-constants.txt \
  --cross-file platform/WebAssembly/wasm-cross.txt \
  --buildtype=release \
  --prefix=$(pwd)/build/DunGen_dist \
  --bindir='' \
  --reconfigure \
  build

cd build
meson install
cd ..

cp docs/acknowledgements.md build/DunGen_dist


## set up cache-busting
cd build/DunGen_dist

# Emscripten generates a new UUID for the data file every time it builds. :/
#  This makes the generated .js have a different hash each time, which is silly.
#  So far as I can tell, the UUID doesn't get used for anything, so swapping it
#  out to something stable helps maintain the cache when appropriate.

# Nothing magical about this UUID; it's just the one that was last generated when
#  I noticed the issue.
uuid=2859d42b-7ce1-4e0f-9679-70e77052e11e

sed -i.bak -e "s/package_uuid:\"[0-9a-zA-Z-]*\"/package_uuid:\"$uuid\"/g" DunGen.js
rm DunGen.js.bak

declare -A busts
busts[DunGen.data]=DunGen.data
busts[DunGen.js]=DunGen.js
busts[DunGen.wasm]=DunGen.wasm
busts[DunGen.html]=DunGen.html

targets=(DunGen.html DunGen.js)

# change filenames and url references to everything except the HTML
for b in ${!busts[@]}; do
    hash=$(md5sum ./$b | awk '{print $1}')
    ext=${b##*.}
    newname=${b%.*}.$hash.$ext
    if [[ $ext != "html" ]]; then
        busts[$b]=$newname
        mv ./$b ./$newname
    fi

    for t in ${targets[@]}; do
        sed -i.bak -e "s:$b:$newname:g" ${busts[$t]}
        rm ./${busts[$t]}.bak
    done
done
