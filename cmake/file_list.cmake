file(GLOB ProjectFiles
        "${CMAKE_CURRENT_SOURCE_DIR}/Inc/.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/Inc/cpp/.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/Src/*.c"
        "${CMAKE_CURRENT_SOURCE_DIR}/Src/cpp/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Src/cpp/MemoryController/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Src/cpp/Devices/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Src/cpp/InterfaceWrappers/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Inc/cpp/MemoryModules/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/Src/cpp/MemoryModules/*.cpp")


set(MainFilesCPP
        "${CMAKE_CURRENT_SOURCE_DIR}/Src/cpp/Main/Main.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Inc/cpp/Main/Main.hpp"
        )

if (BUILD_FOR_STM32)
    file(GLOB_RECURSE MiddlewareFiles
            "${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Inc/*h"
            "${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Src/*c"
            "${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc/*h"
            "${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/*c")

    file(GLOB_RECURSE DriverFiles
            "${CMAKE_CURRENT_SOURCE_DIR}/Src/SystemFiles/*.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/Inc/SystemFiles/*.h"
            "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/USB_HOST/App"
            "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/USB_HOST/Target"
            "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Source/*c"
            "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/${BoardClass}xx_HAL_Driver/Inc/*h"
            "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/${BoardClass}xx_HAL_Driver/Src/*c")

    set(StartUpFile ${CMAKE_CURRENT_SOURCE_DIR}/startup/startup_${BoardType}xx.s)

    set(include_dirs "${CMAKE_CURRENT_SOURCE_DIR}/Inc"
            "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/${BoardClass}xx_HAL_Driver/Inc"
            "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/${BoardClass}xx_HAL_Driver/Inc/Legacy"
            "${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Inc"
            "${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc"
            "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/${BoardClass}xx/Include"
            "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Include"
            "${CMAKE_CURRENT_SOURCE_DIR}")
endif () # BUILD_FOR_STM32