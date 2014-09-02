project(LunaWebAppManager)

cmake_minimum_required(VERSION 2.8.8)

include(webOS/webOS)
webos_modules_init(1 0 0 QUALIFIER RC3)
webos_component(0 2 0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++0x -g")

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
pkg_check_modules(SYSTEMD libsystemd REQUIRED)
pkg_check_modules(PBNJSON_C pbnjson_c REQUIRED)
pkg_check_modules(LUNA_SYSMGR_COMMON LunaSysMgrCommon REQUIRED)
pkg_check_modules(LUNA_PREFS luna-prefs REQUIRED)
pkg_check_modules(CONNMAN_QT5 connman-qt5 REQUIRED)

set(WITH_UNMODIFIED_QTWEBKIT FALSE CACHE BOOL "Set to TRUE to compiled with an unmodified qtwebkit version")
if(WITH_UNMODIFIED_QTWEBKIT)
    add_definitions(-DWITH_UNMODIFIED_QTWEBKIT)
endif()

add_subdirectory(lib)
include_directories(lib)
add_subdirectory(src)

webos_build_configured_file(files/pkgconfig/webapp-plugin.pc PKGCONFIGDIR "")
