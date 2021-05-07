#!/bin/bash

mkdir -p ./tmp
mkdir -p ./bin
pushd ./tmp;
cmake .. -DBoardName="stm32f429"  -DBoardClass=STM32F4 -DCPU=cortex-m4 -DFPUType=hard -DFPUSpecification=fpv4-sp-d16 -Dspecs=rdimon.specs
make
make install
popd;
