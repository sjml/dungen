#!/usr/bin/env bash

set -e

cd "$(dirname "$0")"

source ~/emsdk/emsdk_env.sh

pushd ../build

emcmake cmake -DCMAKE_BUILD_TYPE=MinSizeRel ..
cmake --build .
mkdir -p site
mv DunGen.{html,js,wasm,data} ./site
mv ./site/DunGen.html ./site/index.html

popd
