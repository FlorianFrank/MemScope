file(GLOB ProjectFiles
        "${DEP_ROOT_DIR}/Inc/.h"
        "${DEP_ROOT_DIR}/Inc/cpp/.h"
        "${DEP_ROOT_DIR}/Src/*.c"
        "${DEP_ROOT_DIR}/Src/cpp/*.cpp"
        "${DEP_ROOT_DIR}/Src/cpp/MemoryController/*.cpp"
        "${DEP_ROOT_DIR}/Src/cpp/Devices/*.cpp"
        "${DEP_ROOT_DIR}/Src/cpp/Experiments/*.cpp"
        "${DEP_ROOT_DIR}/Src/cpp/InterfaceWrappers/*.cpp"
        "${DEP_ROOT_DIR}/Inc/cpp/MemoryModules/*.h"
        "${DEP_ROOT_DIR}/Src/cpp/MemoryModules/*.cpp")

set(MainFilesCPP
        "${DEP_ROOT_DIR}/Src/cpp/Main/Main.cpp"
        "${DEP_ROOT_DIR}/Inc/cpp/Main/Main.hpp"
        )


if (BUILD_FOR_STM32)
    file(GLOB_RECURSE MiddlewareFiles
            "${DEP_ROOT_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Inc/*h"
            "${DEP_ROOT_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Src/*c"
            "${DEP_ROOT_DIR}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc/*h"
            "${DEP_ROOT_DIR}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/*c")


    file(GLOB_RECURSE DriverFiles
            "${DEP_ROOT_DIR}/Src/SystemFiles/*.c"
            "${DEP_ROOT_DIR}/Inc/SystemFiles/*.h"
            "${DEP_ROOT_DIR}/Drivers/USB_HOST/App"
            "${DEP_ROOT_DIR}/Drivers/USB_HOST/Target"
            "${DEP_ROOT_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Source/*c"
            "${DEP_ROOT_DIR}/Drivers/${BoardClass}xx_HAL_Driver/Inc/*h"
            "${DEP_ROOT_DIR}/Drivers/${BoardClass}xx_HAL_Driver/Src/*c")

    set(StartUpFile ${DEP_ROOT_DIR}/startup/startup_${BoardType}xx.s)

    set(include_dirs "${DEP_ROOT_DIR}/Inc"
            "${DEP_ROOT_DIR}/Drivers/${BoardClass}xx_HAL_Driver/Inc"
            "${DEP_ROOT_DIR}/Drivers/${BoardClass}xx_HAL_Driver/Inc/Legacy"
            "${DEP_ROOT_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Inc"
            "${DEP_ROOT_DIR}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc"
            "${DEP_ROOT_DIR}/Drivers/CMSIS/Device/ST/${BoardClass}xx/Include"
            "${DEP_ROOT_DIR}/Drivers/CMSIS/Include"
            "${DEP_ROOT_DIR}")
endif () # BUILD_FOR_STM32