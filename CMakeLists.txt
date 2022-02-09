cmake_minimum_required(VERSION 3.1)

# Set build options
option(BUILD_UNIT_TESTS "Build gtest unittests" OFF)
option(BUILD_DOCUMENTATION "Build documentation" OFF)
option(BUILD_EXECUTABLE "Build Executable" ON)
option(BUILD_LIB "Build library" ON)

# Set architecture options
option(BUILD_FOR_STM32 "Build for STM32 Devices?" ON)
option(BUILD_FOR_STM32F429 "Build for STM32 Devices?" ON)
option(BUILD_HOST_MACHINE "Build for current host machine" OFF)
option(LINK_STM32_USB_LIB "Link the USB lib to the repository" OFF)
option (ENABLE_RDIMON_SPECS "Enable rdimon.specs" ON)

set(ProgramName "MemoryController")
set(LibraryName "MemoryController_library")

project(${ProgramName} ASM CXX C )

if(ENABLE_RDIMON_SPECS)
	message("Enable rdimon.specs")
	add_definitions(-DRDMON_SPECS=1)
endif() #rdimon.specs

if(BUILD_FOR_STM32)
	set(ExecutableName "${ProgramName}.elf")
	add_definitions(-DSTM32=1)

	if(BUILD_FOR_STM32F429)
		set(CMAKE_TOOLCHAIN_FILE "toolchains/toolchain_stm32f429.cmake")
		add_definitions(-DSTM32F429xx=1)
		set(BoardType "STM32F429")
		set(BoardClass "STM32F4")
	endif()

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

	set_property(SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/startup/startup_${BoardType}xx.s PROPERTY LANGUAGE C)
	message(${CMAKE_CURRENT_SOURCE_DIR}/startup/startup_${BoardType}xx.s)
	if(BUILD_FOR_STM32F429)
		set(StartUpFile ${CMAKE_CURRENT_SOURCE_DIR}/startup/startup_${BoardType}xx.s)
		message(${StartUpFile})
	endif() #BUILD_FOR_STM32F429

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

	if(LINK_STM32_USB_LIB)
		add_subdirectory(stm32_usb_lib)
		set(USB_LIB STM32_USB_Lib)
	endif()

	set(INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Inc"
			"${CMAKE_CURRENT_SOURCE_DIR}/Drivers/${BoardClass}xx_HAL_Driver/Inc"
			"${CMAKE_CURRENT_SOURCE_DIR}/Drivers/${BoardClass}xx_HAL_Driver/Inc/Legacy"
			"${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Core/Inc"
			"${CMAKE_CURRENT_SOURCE_DIR}/Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc"
			"${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/${BoardClass}xx/Include"
			"${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Include"
			"${CMAKE_CURRENT_SOURCE_DIR}")

	message(${CMAKE_CURRENT_SOURCE_DIR}/Drivers/${BoardClass}xx_HAL_Driver/Inc)

	if(BUILD_EXECUTABLE)
		add_executable(${ExecutableName}   ${MiddlewareFiles} ${StartUpFile} ${DriverFiles}  ${ProjectFiles} ${MainFiles}   )
		target_include_directories(${ExecutableName} PRIVATE ${INCLUDE_DIRS})
		install(TARGETS ${ExecutableName} RUNTIME DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/bin")
		if(ENABLE_RDIMON_SPECS)
			target_link_libraries(${ExecutableName} c rdimon ${USB_LIB})
		endif()
		set(CMAKE_OBJCOPY arm-none-eabi-objcopy)
		add_custom_command(TARGET ${ExecutableName} POST_BUILD
				COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${ExecutableName}> ${HEX_FILE}
				COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${ExecutableName}> ${BIN_FILE}
				COMMENT "Building ${HEX_FILE} \nBuilding ${BIN_FILE}")
	endif() # BUILD_EXECUTABLE

	if(BUILD_LIB)
		add_library(${LibraryName} ${ProjectFiles}  ${MiddlewareFiles} ${DriverFiles} ${StartUpFile})
		target_include_directories(${LibraryName} PRIVATE ${INCLUDE_DIRS})
		install(TARGETS ${LibraryName} RUNTIME DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/bin")
		if(ENABLE_RDIMON_SPECS)
			target_link_libraries(${LibraryName} c rdimon ${USB_LIB})
		endif()
	endif()

	set(HEX_FILE ${BuildFolder}/${ProgramName}.hex)
	set(BIN_FILE ${BuildFolder}/${ProgramName}.bin)
endif() # BULD_STM32

if(BUILD_HOST_MACHINE)
	if(BUILD_EXECUTABLE)
		set(ExecutableName "${ProgramName}")
		add_executable(${ExecutableName} ${ProjectFiles} ${MainFiles}  ${MiddlewareFiles} ${DriverFiles} ${StartUpFile})
		target_include_directories(${ExecutableName} PRIVATE
				"${CMAKE_CURRENT_SOURCE_DIR}/Inc"
				"${CMAKE_CURRENT_SOURCE_DIR}")
		install(TARGETS ${ExecutableName} RUNTIME DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/bin")
	endif()

	if(BUILD_LIB)
		add_executable(${LibraryName} ${ProjectFiles} ${MiddlewareFiles} ${DriverFiles} ${StartUpFile})
		target_include_directories(${LibraryName} PRIVATE
				"${CMAKE_CURRENT_SOURCE_DIR}/Inc"
				"${CMAKE_CURRENT_SOURCE_DIR}")
		install(TARGETS ${LibraryName} RUNTIME DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/bin")
	endif()
endif()


if($ENV{CLION_IDE})
	set(BuildFolder "${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug")
else() # ! CLION_IDE
	set(BuildFolder "${CMAKE_CURRENT_SOURCE_DIR}/tmp")
endif() # if CLION_IDE


if (BUILD_UNIT_TESTS)
	#unit testing

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