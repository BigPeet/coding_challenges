#!/bin/bash

mkdir build
cd build
cmake .. -DBUILD_TESTING=1 -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=RelWithDebInfo
make
cd ..
ln -s build/compile_commands.json .
