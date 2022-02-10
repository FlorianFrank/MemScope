set(EXEC_NAME "MemoryController")
set(LIB_NAME "MemoryController_library")
set(INSTALL_DIR "bin")

# Set build options
option(BUILD_UNIT_TESTS "Build gtest unittests" OFF)
option(BUILD_DOCUMENTATION "Build documentation" OFF)
option(BUILD_EXECUTABLE "Build Executable" ON)
option(BUILD_LIB "Build library" OFF)

# Set architecture options
option(BUILD_FOR_STM32 "Build for STM32 Devices?" ON)
option(BUILD_FOR_STM32F429 "Build for STM32F429 Device?" ON)
option (ENABLE_RDIMON_SPECS "Enable rdimon.specs" ON)

# build for local machine
option(BUILD_HOST_MACHINE "Build for currentf host machine" OFF)

# compile additional libs
option(LINK_STM32_USB_LIB "Link the USB lib to the repository" OFF)
