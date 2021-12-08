#!/bin/bash

mkdir -p build
pushd build
cmake .. -DUNITTEST=1
make
popd
