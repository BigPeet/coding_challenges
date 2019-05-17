#!/bin/bash

cd build
cmake .. -DBUILD_TESTING=1 -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
make
cd ..
ln -s build/compile_commands.json .
