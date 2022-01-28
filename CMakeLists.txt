cmake_minimum_required(VERSION 2.9)

#set(STM32 1)
set(UNIT_TESTING 1)
set(ProgramName "Memory_Testing_FW")
option(BUILD_DOCUMENTATION "Build documentation" OFF)

# Variables
#set(BoardName  "stm32f429")
#set(BoardClass "STM32F4")
#set(CPU "cortex-m4")
#set(FPUType "hard")
#set(FPUSpecification "fpv4-sp-d16")
#set(specs "rdimon.specs")
#-DBoardName="stm32f429"  -DBoardClass=STM32F4 -DCPU=cortex-m4 -DFPUType=hard -DFPUSpecification=fpv4-sp-d16 -Dspecs=rdimon.specs

if(STM32)
	set(FullProgramName "${ProgramName}.elf")
	set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/toolchain.cmake")
	add_definitions(-DSTM32F429xx=1 -DSTM32=1)
else()
		set(FullProgramName "${ProgramName}")
endif()

project(${ProgramName} CXX C ASM)

set(MainFiles
		"${CMAKE_CURRENT_SOURCE_DIR}/Inc/cpp/Main/Main.h"
		"${CMAKE_CURRENT_SOURCE_DIR}/Src/cpp/Main/Main.cpp"
		)

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

file(GLOB_RECURSE StartupFiles
	"${CMAKE_CURRENT_SOURCE_DIR}/startup/.s")

#add_definitions(-DSTM32=1)


if(STM32)
set_property(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/startup/startup_${BoardName}xx.s PROPERTY LANGUAGE C)
set(StartUpFile ${CMAKE_CURRENT_SOURCE_DIR}/startup/startup_${BoardName}xx.s)

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
endif() # STM32

if(STM32)
add_executable(${FullProgramName} ${ProjectFiles} ${MainFiles} ${StartupFiles} ${MiddlewareFiles} ${DriverFiles} ${StartUpFile})

	if("${specs}" STREQUAL "rdimon.specs" )
		message("Enable rdimon.specs")
		add_definitions(-DRDMON_SPECS=1)
		target_link_libraries(${FullProgramName} c rdimon) # STM32_USB_Lib TODO
	endif() # disable syscalls.c
else()
	add_executable(${FullProgramName} ${ProjectFiles} ${MainFiles} ${StartupFiles} ${MiddlewareFiles} ${DriverFiles} ${StartUpFile})
endif() # STM32


if (USB_LIB)
	#add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/stm32_usb_lib")
endif ()



if($ENV{CLION_IDE})
set(BuildFolder "${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug")
else() # ! CLION_IDE
	set(BuildFolder "${CMAKE_CURRENT_SOURCE_DIR}/tmp")
endif() # if CLION_IDE

set(HEX_FILE ${BuildFolder}/${ProgramName}.hex)
set(BIN_FILE ${BuildFolder}/${ProgramName}.bin)

if(STM32)
# Generate the hex and binary files
set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
add_custom_command(TARGET ${ProgramName}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${ProgramName}.elf> ${HEX_FILE}
        COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${ProgramName}.elf> ${BIN_FILE}
        COMMENT "Building ${HEX_FILE} \nBuilding ${BIN_FILE}")
endif()

target_include_directories(${FullProgramName} PRIVATE
		            "${CMAKE_CURRENT_SOURCE_DIR}/Inc"
					"${CMAKE_CURRENT_SOURCE_DIR}/Drivers/${BoardClass}xx_HAL_Driver/Inc"
					"${CMAKE_CURRENT_SOURCE_DIR}/Drivers/${BoardClass}xx_HAL_Driver/Inc/Legacy"
					"${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Inc"
					"${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc"
					"${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/${BoardClass}xx/Include"
					"${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Include"
					"${CMAKE_CURRENT_SOURCE_DIR}")

install(TARGETS ${FullProgramName} RUNTIME DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/bin")


if (UNIT_TESTING)
#unit testing
cmake_minimum_required(VERSION 2.8)

project(MemoryControllerTest)

add_executable(MemoryControllerTest ${ProjectFiles}
		"Tests/Src/SPIMemoryControllerTest.cpp"
		"Tests/Src/TestInterfaceWrapper.cpp"
		)

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
		"${CMAKE_CURRENT_SOURCE_DIR}/../Inc"
		"${CMAKE_CURRENT_SOURCE_DIR}")
enable_testing()
include(GoogleTest)
gtest_discover_tests("MemoryControllerTest")

add_definitions(-DUNIT_TEST=1)
endif() # UNIT TESTING



# check if Doxygen is installed
if(BUILD_DOCUMENTATION)
	find_package(Doxygen)
	if (DOXYGEN_FOUND)
		# set input and output files
		set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
		set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/doc)

		# request to configure the file
		configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
		message("Doxygen build started")

		add_custom_target( doc_doxygen ALL
				COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
				WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
				COMMENT "Generating API documentation with Doxygen"
				VERBATIM )

	else (DOXYGEN_FOUND)
		message("Doxygen need to be installed to generate the doxygen documentation")
	endif (DOXYGEN_FOUND)
endif() # DEFINED BUILD_DOCUMENTATION