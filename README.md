# STM_Measurement_Firmware

## Run in CLion
1. Check this [official post](https://www.jetbrains.com/help/clion/embedded-development.html#new-project)
    about STM32CubeMX projects in CLion (Do not need the OpenOCD + STM32CubeMX plugin).
2. Git clone this project.
3. Open(file --> Open...) this project in CLion
4. Add ocd debug 
    1. Go to Run/Debug Configuration
    2. Add New OpenOCD Download & Run
    3. Select Target memory_control_fw.elf
    4. Select Executable memory_control_fw.elf
    5. Select Board Config File (stm32f429disc1.cfg in this project)
    6. Select Download Always
    7. Select Reset Halt
5. Change Cmake options
    1. Go to File → Settings → Build, Execution, Deployment → Cmake
    2. Add    
       -DBoardName="stm32f429" -DBoardClass=STM32F4 -DCPU=cortex-m4 -DFPUType=hard -DFPUSpecification=fpv4-sp-d16 -Dspecs=rdimon.specs   
       to Cmake options → click Apply and Ok
6. Build & Debug (test with flash_leds function in gpio.c file).
