#!/usr/bin/env bash

cd "$(dirname "$0")"
cd ..

type swig >/dev/null 2>&1 || exit 1

BASEDIR=$(pwd)
swig -lua -Werror -o ${BASEDIR}/src/scripting/wrapping.c ${BASEDIR}/src/scripting/interfaces/dungen.i
