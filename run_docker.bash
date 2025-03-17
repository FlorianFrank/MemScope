#!/bin/zsh

#docker start stm32f429-build
docker run --rm -v "/Users/florian/Documents/Arbeit/Projekte/pufmem/code/memory_controller_firmware:/project" stm32f429-build /project/compile_fw_STM32F4.bash