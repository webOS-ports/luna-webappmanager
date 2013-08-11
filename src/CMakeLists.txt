include_directories(
    ${Qt5WebKit_PRIVATE_INCLUDE_DIRS}
    ${Qt5Quick_PRIVATE_INCLUDE_DIRS}
    ${GLIB2_INCLUDE_DIRS}
    ${LS2_INCLUDE_DIRS}
    ${PBNJSON_C_INCLUDE_DIRS})

set(SOURCES
    main.cpp
    utils.cpp
    webappmanager.cpp
    webappmanagerservice.cpp
    lunaserviceutils.cpp
    webapplication.cpp
    applicationdescription.cpp)

set(HEADERS
    utils.h
    webappmanager.h
    webappmanagerservice.h
    lunaserviceutils.h
    webapplication.h
    applicationdescription.h)

qt5_add_resources(RESOURCES resources.qrc)

add_executable(WebAppMgr ${SOURCES} ${HEADERS} ${RESOURCES})
qt5_use_modules(WebAppMgr Quick Gui WebKit)
target_link_libraries(WebAppMgr
    ${LS2_LIBRARIES}
    ${GLIB2_LIBRARIES}
    ${SYSTEMD_DAEMON_LIBRARIES}
    ${PBNJSON_C_LIBRARIES})

webos_build_daemon()
webos_build_system_bus_files()
