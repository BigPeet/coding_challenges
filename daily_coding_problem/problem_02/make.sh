#!/usr/bin/env bash

if [[ ! -d ./build ]]; then
  mkdir build
fi

cd build
cmake .. -DBUILD_TESTING=1 -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo
make
cd ..

if [[ ! -f compile_commands.json ]]; then
  ln -s build/compile_commands.json .
fi
