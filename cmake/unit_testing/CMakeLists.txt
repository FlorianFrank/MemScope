function(build_unit_tests)

    project(MemoryControllerTest)

    add_executable(MemoryControllerTest ${ProjectFiles}
            "Tests/Src/SPIMemoryControllerTest.cpp"
            "Tests/Src/TestInterfaceWrapper.cpp"
            )

    set_target_properties(MemoryControllerTest PROPERTIES LINKER_LANGUAGE CXX)


    target_include_directories(MemoryControllerTest PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/Inc"
            "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/${BoardClass}xx_HAL_Driver/Inc"
            "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/${BoardClass}xx_HAL_Driver/Inc/Legacy"
            "${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Inc"
            "${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc"
            "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/${BoardClass}xx/Include"
            "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Include"
            "${CMAKE_CURRENT_SOURCE_DIR}/Tests/Inc")

    target_link_libraries(MemoryControllerTest gtest_main gtest pthread)
    target_include_directories(MemoryControllerTest PRIVATE
            "../../Inc"
            "..")
    enable_testing()
    include(GoogleTest)
    gtest_discover_tests("MemoryControllerTest")

    add_definitions(-DUNIT_TEST=1)
endfunction()