#!/usr/bin/env bash
git submodule init
git submodule update --progress
mkdir build
cd build
cmake ..
make -j12
