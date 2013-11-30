include_directories(
    ${Qt5WebKit_PRIVATE_INCLUDE_DIRS}
    ${Qt5Quick_PRIVATE_INCLUDE_DIRS}
    ${GLIB2_INCLUDE_DIRS}
    ${LS2_INCLUDE_DIRS}
    ${PBNJSON_C_INCLUDE_DIRS}
    ${LUNA_SYSMGR_COMMON_INCLUDE_DIRS})

set(SOURCES
    main.cpp
    utils.cpp
    webapplauncher.cpp
    webapplication.cpp
    webapplicationwindow.cpp
    applicationdescription.cpp
    plugins/baseplugin.cpp
    plugins/palmsystemplugin.cpp)

set(HEADERS
    utils.h
    webapplauncher.h
    webapplication.h
    webapplicationwindow.h
    applicationdescription.h
    plugins/baseplugin.h
    plugins/palmsystemplugin.h)

qt5_add_resources(RESOURCES resources.qrc)

# Install framework scripts for the case we're running on an unpatched qtwebkit
set(WEBOS_FRAMEWORK qml/webos-api.js)
install (FILES ${WEBOS_FRAMEWORK} DESTINATION ${WEBOS_INSTALL_WEBOS_FRAMEWORKSDIR}/webos)

add_executable(webapp-launcher ${SOURCES} ${HEADERS} ${RESOURCES})
qt5_use_modules(webapp-launcher Quick Gui WebKit)
target_link_libraries(webapp-launcher
    ${LS2_LIBRARIES}
    ${GLIB2_LIBRARIES}
    ${SYSTEMD_DAEMON_LIBRARIES}
    ${PBNJSON_C_LIBRARIES}
    ${LUNA_SYSMGR_COMMON_LIBRARIES})

webos_build_program(ADMIN)
webos_build_system_bus_files()
