if (BUILD_FOR_STM32)
    add_definitions(-DSTM32=1)

    if (BUILD_FOR_STM32F429)
        set(CMAKE_TOOLCHAIN_FILE "cmake/toolchains/toolchain_stm32f429.cmake")
        add_definitions(-DSTM32F429xx=1)
        set(BoardType "STM32F429")
        set(BoardClass "STM32F4")
    endif ()

    if(ENABLE_RDIMON_SPECS)
        message("Enable rdimon.specs")
        add_definitions(-DRDMON_SPECS=1)
    endif() #rdimon.specs

    set(EXEC_NAME "${EXEC_NAME}.elf")
    set_property(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/startup/startup_${BoardType}xx.s PROPERTY LANGUAGE C)
endif () # BUILD_FOR_STM32


# include helper cmake files
if (BUILD_HOST_MACHINE)
    set(CMAKE_TOOLCHAIN_FILE "cmake/toolchains/toolchain_default_gcc.cmake")
endif ()