#!/usr/bin/env bash

cd "$(dirname "$0")"
cd ..
cd build/DunGen_dist

declare -A busts
busts[DunGen.data]=DunGen.data
busts[DunGen.js]=DunGen.js
busts[DunGen.wasm]=DunGen.wasm
busts[DunGen.html]=DunGen.html

targets=(DunGen.html DunGen.js)

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