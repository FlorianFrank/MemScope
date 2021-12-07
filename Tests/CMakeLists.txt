cmake_minimum_required(VERSION 2.8)

project(MemoryControllerTest)

set(PROJECT_FILES "../Inc/cpp/MemoryController.h"
                  "../Src/cpp/MemoryController.cpp"

                  "../Inc/cpp/TimeMeasurement.h"
                  "../Src/cpp/TimeMeasurement.cpp"

                  "../Inc/cpp/InterfaceWrappers/SPIWrapper.h"
                  "../Src/cpp/InterfaceWrappers/SPIWrapper.cpp"

                  "../Inc/cpp/MemoryControllerWrappers/MemoryControllerSPI.h"
                  "../Src/cpp/MemoryControllerWrappers/MemoryControllerSPI.cpp"

                  "../Inc/cpp/STMHelperFunctions.h"
                  "../Src/cpp/STMHelperFunctions.cpp"

                  "../Inc/metrics.h"
                  "../Src/metrics.c"
                    )

add_executable(MemoryControllerTest ${PROJECT_FILES}
            "SPIMemoryControllerTest.cpp")

    target_link_libraries(MemoryControllerTest gtest_main gtest pthread)
    target_include_directories(MemoryControllerTest PRIVATE "../Inc")
    enable_testing()
    include(GoogleTest)
    gtest_discover_tests("MemoryControllerTest")