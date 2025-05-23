#!/bin/bash

rm -r ./tmp
rm -r ./bin

mkdir -p ./tmp
mkdir -p ./bin
pushd ./tmp
cmake .. -DBoardName="stm32f429"  -DBoardClass=STM32F4 -DCPU=cortex-m4 -DFPUType=hard -DFPUSpecification=fpv4-sp-d16 -Dspecs=rdimon.specs -DOS_USE_SEMIHOSTING=1
make
make install
popd