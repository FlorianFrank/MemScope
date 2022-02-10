cmake_minimum_required(VERSION 3.1)

# include helper cmake files
include(${CMAKE_CURRENT_SOURCE_DIR}/settings.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/device_specific_defines.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/file_list.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/build_documentation.cmake)

project(${EXEC_NAME} CXX C ASM)

#
# *** Function to build a lib or executable and to link the corresponding directories and libraries *******
#
function(build_lib_exe type build_name)
    if (${type} STREQUAL "exec")
        message("Build executable")
        build_lib_exe("lib" ${LIB_NAME})
        add_executable(${build_name} ${StartUpFile} ${MainFilesCPP} )
        target_link_libraries(${build_name}  ${LIB_NAME})
    else ()
        message("Build library")
        add_library(${build_name} ${ProjectFiles} ${MiddlewareFiles} ${DriverFiles} ${StartUpFile})
    endif()

    target_include_directories(${build_name} PUBLIC ${include_dirs})
    install(TARGETS ${build_name} RUNTIME DESTINATION "${CMAKE_CURRENT_SOURCE_DIR}/${INSTALL_DIR}")

    if (ENABLE_RDIMON_SPECS)
        target_link_libraries(${build_name} c rdimon ${USB_LIB})
    else ()
        target_link_libraries(${build_name} ${USB_LIB})
    endif ()
endfunction()


if (BUILD_FOR_STM32)
    if (LINK_STM32_USB_LIB)
        add_subdirectory(stm32_usb_lib)
        set(USB_LIB STM32_USB_Lib)
    endif ()
endif () # BULD_STM32

if (BUILD_EXECUTABLE)
    build_lib_exe("exec" "${EXEC_NAME}")
endif () # BUILD_EXECUTABLE

if (BUILD_LIB)
    build_lib_exe("lib" "${LIB_NAME}")
endif () # BUILD_LIB

if (BUILD_UNIT_TESTS)
    add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/cmake/unit_testing")
endif () # BUILD_UNIT_TESTS

if (BUILD_DOCUMENTATION)
    build_documentation()
endif () # BUILD_DOCUMENTATION

if ($ENV{CLION_IDE})
    set(BuildFolder "${CMAKE_CURRENT_SOURCE_DIR}/cmake-build-debug")
else () # ! CLION_IDE
    set(BuildFolder "${CMAKE_CURRENT_SOURCE_DIR}/tmp")
endif () # if CLION_IDE
