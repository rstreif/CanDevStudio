#!/usr/bin/env bash

set -e #Exit on first error

#Configure, build and test
source /opt/qt56/bin/qt56-env.sh
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j4 
