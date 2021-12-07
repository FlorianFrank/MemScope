cmake_minimum_required(VERSION 2.9)

set(ProgramName "Memory_Testing_FW")

# Variables
#set(BoardName  "stm32f429")
#set(BoardClass "STM32F4")
#set(CPU "cortex-m4")
#set(FPUType "hard")
#set(FPUSpecification "fpv4-sp-d16")
#set(specs "rdimon.specs")

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/toolchain.cmake")
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)

project(${ProgramName} CXX C ASM)


file(GLOB_RECURSE ProjectFiles 
	"${CMAKE_CURRENT_SOURCE_DIR}/Inc/.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Inc/cpp/.h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Src/*.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/Src/cpp/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Src/cpp/MemoryControllerWrappers/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/Src/cpp/InterfaceWrappers/*.cpp")

file(GLOB_RECURSE StartupFiles
	"${CMAKE_CURRENT_SOURCE_DIR}/startup/.s")

set_property(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/startup/startup_${BoardName}xx.s PROPERTY LANGUAGE C)
set(StartUpFile ${CMAKE_CURRENT_SOURCE_DIR}/startup/startup_${BoardName}xx.s)

file(GLOB_RECURSE MiddlewareFiles
	"${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Inc/*h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Src/*c"
	"${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc/*h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/*c")

file(GLOB_RECURSE DriverFiles
	"${CMAKE_CURRENT_SOURCE_DIR}/Drivers/USB_HOST/App"
	"${CMAKE_CURRENT_SOURCE_DIR}/Drivers/USB_HOST/Target"
	"${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F4xx/Source/*c"
	"${CMAKE_CURRENT_SOURCE_DIR}/Drivers/${BoardClass}xx_HAL_Driver/Inc/*h"
	"${CMAKE_CURRENT_SOURCE_DIR}/Drivers/${BoardClass}xx_HAL_Driver/Src/*c")

add_executable(${PROJECT_NAME}.elf ${ProjectFiles} ${StartupFiles} ${MiddlewareFiles} ${DriverFiles} ${StartUpFile})


if("${specs}" STREQUAL "rdimon.specs")
	message("Enable rdimon.specs")
	add_definitions(-DRDMON_SPECS=1)
	target_link_libraries(${PROJECT_NAME}.elf STM32_USB_Lib c rdimon)
else()
	target_link_libraries(${PROJECT_NAME}.elf STM32_USB_Lib)
endif() # disable syscalls.c

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/stm32_usb_lib")


if($ENV{CLION_IDE})
set(BuildFolder "${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug")
else() # ! CLION_IDE
	set(BuildFolder "${CMAKE_CURRENT_SOURCE_DIR}/tmp")
endif() # if CLION_IDE

set(HEX_FILE ${BuildFolder}/${ProgramName}.hex)
set(BIN_FILE ${BuildFolder}/${ProgramName}.bin)

# Generate the hex and binary files
add_custom_command(TARGET ${ProgramName}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${ProgramName}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${ProgramName}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE} \nBuilding ${BIN_FILE}")


target_include_directories(${PROJECT_NAME}.elf PRIVATE
		            "${CMAKE_CURRENT_SOURCE_DIR}/Inc"
					"${CMAKE_CURRENT_SOURCE_DIR}/Drivers/${BoardClass}xx_HAL_Driver/Inc"
					"${CMAKE_CURRENT_SOURCE_DIR}/Drivers/${BoardClass}xx_HAL_Driver/Inc/Legacy"
					"${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Inc"
					"${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc"
					"${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/${BoardClass}xx/Include"
					"${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Include"
					"${CMAKE_CURRENT_SOURCE_DIR}/stm32_usb_lib/include")

install(TARGETS ${PROJECT_NAME}.elf RUNTIME DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/bin")
