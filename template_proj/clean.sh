#!/usr/bin/env bash

if [[ -d ./build ]]; then
  rm -rf ./build
fi

if [[ -f compile_commands.json ]]; then
  rm compile_commands.json
fi
