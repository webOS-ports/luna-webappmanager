add_subdirectory(webkitbundle)

include_directories(
    ${Qt5WebKit_PRIVATE_INCLUDE_DIRS}
    ${Qt5Quick_PRIVATE_INCLUDE_DIRS}
    ${GLIB2_INCLUDE_DIRS}
    ${LS2_INCLUDE_DIRS})

set(HEADERS
    webappmanager.h)

set(SOURCES
    main.cpp
    webappmanager.cpp)

add_executable(luna-webappmanager ${SOURCES})
qt5_use_modules(luna-webappmanager Quick Gui WebKit)

target_link_libraries(luna-webappmanager
    ${LS2_LIBRARIES}
    ${SYSTEMD_DAEMON_LIBRARIES})

install(TARGETS luna-webappmanager DESTINATION bin)
