#!/bin/bash

cd build
cmake .. -DBUILD_TESTING=1
make
cd ..
