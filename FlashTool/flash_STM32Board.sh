#!/bin/bash

/opt/st/stm32cubeide_1.5.0/plugins/com.st.stm32cube.ide.mcu.externaltools.openocd.linux64_1.5.0.202011040924/tools/bin/openocd -d0 -f "openocd_cfg/config.cfg" -c "program \"../bin/Memory_Testing_FW.elf\""  #-c "flash write_image_erase tmp/Memory_Testing_FW.hex"
