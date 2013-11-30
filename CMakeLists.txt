project(webapp-launcher)

cmake_minimum_required(VERSION 2.8.8)

include(webOS/webOS)
webos_modules_init(1 0 0 QUALIFIER RC3)
webos_component(0 1 0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden -fvisibility-inlines-hidden -Wall -std=c++0x")

find_package(Qt5Gui REQUIRED)
if(NOT Qt5Gui_FOUND)
    message(FATAL_ERROR "Qt5Gui module is required!")
endif()

find_package(Qt5Qml REQUIRED)
if(NOT Qt5Qml_FOUND)
    message(FATAL_ERROR "Qt5Qml module is required!")
endif()

find_package(Qt5Quick REQUIRED)
if(NOT Qt5Quick_FOUND)
    message(FATAL_ERROR "Qt5Quick module is required!")
endif()

find_package(Qt5WebKit REQUIRED)
if(NOT Qt5WebKit_FOUND)
    message(FATAL_ERROR "Qt5WebKit module is required!")
endif()

find_package(PkgConfig "0.22" REQUIRED)

pkg_check_modules(GLIB2 glib-2.0 REQUIRED)
pkg_check_modules(LS2 luna-service2 REQUIRED)
pkg_check_modules(SYSTEMD_DAEMON libsystemd-daemon REQUIRED)
pkg_check_modules(PBNJSON_C pbnjson_c REQUIRED)
pkg_check_modules(LUNA_SYSMGR_COMMON LunaSysMgrCommon REQUIRED)

add_subdirectory(src)
