cmake_minimum_required(VERSION 3.14)

project(STM32FlashTool VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package(...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()
set(CMAKE_PREFIX_PATH "/home/florianfrank/Qt/5.12.11/gcc_64/lib/cmake")


find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick LinguistTools REQUIRED)

set(TS_FILES STM32FlashTool_en_150.ts)

set(PROJECT_SOURCES
        main.cpp
        cpp/memoryconfig.cpp
      #  compilation.cpp
        qml.qrc
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(STM32FlashTool
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(STM32FlashTool SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(STM32FlashTool
          ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_compile_definitions(STM32FlashTool
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(STM32FlashTool
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick pthread)

set_target_properties(STM32FlashTool PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(STM32FlashTool)
    qt_finalize_executable(STM32FlashTool)
endif()
