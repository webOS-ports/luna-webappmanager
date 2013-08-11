add_subdirectory(webkitbundle)

include_directories(
    ${Qt5WebKit_PRIVATE_INCLUDE_DIRS}
    ${Qt5Quick_PRIVATE_INCLUDE_DIRS}
    ${GLIB2_INCLUDE_DIRS}
    ${LS2_INCLUDE_DIRS}
    ${PBNJSON_C_INCLUDE_DIRS})

set(HEADERS
    webappmanager.h
    webappmanagerservice.h
    lunaserviceutils.h)

set(SOURCES
    main.cpp
    webappmanager.cpp
    webappmanagerservice.cpp
    lunaserviceutils.cpp)

add_executable(luna-webappmanager ${SOURCES})
qt5_use_modules(luna-webappmanager Quick Gui WebKit)

target_link_libraries(luna-webappmanager
    ${LS2_LIBRARIES}
    ${GLIB_LIBRARIES}
    ${SYSTEMD_DAEMON_LIBRARIES}
    ${PBNJSON_C_LIBRARIES})

install(TARGETS luna-webappmanager DESTINATION bin)
