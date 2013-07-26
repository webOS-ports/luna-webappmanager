include_directories(
    ${Qt5WebKit_PRIVATE_INCLUDE_DIRS}
    ${GLIB2_INCLUDE_DIRS}
    ${LS2_INCLUDE_DIRS})

set(HEADERS
    webkitbundle.h)

set(SOURCES
    plugin.cpp
    webkitbundle.cpp)

add_library(lunawebkitbundle SHARED ${SOURCES})
target_link_libraries(lunawebkitbundle ${LS2_LIBRARIES})
qt5_use_modules(lunawebkitbundle WebKit)
