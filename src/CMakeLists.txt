include_directories(
    ${Qt5WebKit_PRIVATE_INCLUDE_DIRS}
    ${Qt5Quick_PRIVATE_INCLUDE_DIRS}
    ${GLIB2_INCLUDE_DIRS}
    ${LS2_INCLUDE_DIRS}
#    ${LS2CXX_INCLUDE_DIRS}
    ${PBNJSON_C_INCLUDE_DIRS}
    ${LUNA_SYSMGR_COMMON_INCLUDE_DIRS}
    ${LUNA_SERVIVCE2_INCLUDE_DIRS}
    ${LUNA_PREFS_INCLUDE_DIRS}
    ${CONNMAN_QT5_INCLUDE_DIRS})

set(SOURCES
    main.cpp
    utils.cpp
    webappmanager.cpp
    webappmanagerservice.cpp
    webapplication.cpp
    webapplicationplugin.cpp
    webapplicationwindow.cpp
    applicationdescription.cpp
    activity.cpp
    systemtime.cpp
    extensions/palmsystemextension.cpp
    extensions/deviceinfo.cpp
    extensions/wifimanager.cpp
    extensions/inappbrowserextension.cpp)

set(HEADERS
    utils.h
    webappmanager.h
    webappmanagerservice.h
    webapplication.h
    webapplicationplugin.h
    webapplicationwindow.h
    applicationdescription.h
    activity.h
    systemtime.h
    extensions/palmsystemextension.h
    extensions/deviceinfo.h
    extensions/wifimanager.h
    extensions/inappbrowserextension.h)

qt5_add_resources(RESOURCES resources.qrc)

# Install framework scripts for the case we're running on an unpatched qtwebkit
set(WEBOS_FRAMEWORK qml/webos-api.js)
install (FILES ${WEBOS_FRAMEWORK} DESTINATION ${WEBOS_INSTALL_WEBOS_FRAMEWORKSDIR}/webos)

add_executable(LunaWebAppManager ${SOURCES} ${HEADERS} ${RESOURCES})
qt5_use_modules(LunaWebAppManager Quick Gui WebKit DBus)
target_link_libraries(LunaWebAppManager
    webapp-plugin
    ${LS2_LIBRARIES}
#    ${LS2CXX_LIBRARIES}
    -lluna-service2++
    ${GLIB2_LIBRARIES}
    ${SYSTEMD_LIBRARIES}
    ${PBNJSON_C_LIBRARIES}
    ${LUNA_SYSMGR_COMMON_LIBRARIES}
    ${LUNA_SERVIVCE2_LIBRARIES}
    ${LUNA_PREFS_LIBRARIES}
    ${CONNMAN_QT5_LDFLAGS})

webos_add_compiler_flags(ALL -DQT_NO_SIGNALS_SLOTS_KEYWORDS)
webos_build_program(ADMIN)
webos_build_system_bus_files()
