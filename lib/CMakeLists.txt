set(SOURCES_LIB
    baseextension.cpp
    applicationenvironment.cpp
    baseextension.h
    applicationenvironment.h)

add_library(webapp-plugin SHARED ${SOURCES_LIB})
target_link_libraries(webapp-plugin Qt5::Core)

install(FILES baseextension.h applicationenvironment.h applicationplugin.h DESTINATION include/webapp-plugin)

webos_build_library(NAME libwebapp-plugin TARGET webapp-plugin NOHEADERS)
