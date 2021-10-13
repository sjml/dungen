#!/usr/bin/env bash

tag=$(git tag -l --points-at HEAD)
if [[ ! -z $tag ]]; then
  echo $tag
else
  echo $(git rev-parse --short HEAD)
fi
