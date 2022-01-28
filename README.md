Installation {#mainpage}
=========

## 1. Overall description

This library provides C++ functionallity which is used to communicate with different types of memories to test them according to PUF behavior. 
There exists wrappers for all device specific functions like initialization functions for interfaces like UART or SPI. 
It is also possible to dynamically add memory modules to the library. 

![STM32F429](../../../doc/STM32f429.png)


## 2. Installation

### 2.1 Compile on Command Line

1. Run ./compile_fw_STM32F4.sh to build for a STM32F4 board
2. The 

### 2.1 Run in CLion
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
