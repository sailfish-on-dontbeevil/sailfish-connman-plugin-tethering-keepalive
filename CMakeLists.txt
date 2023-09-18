cmake_minimum_required(VERSION 3.6.0)

project(sailfish-connman-plugin-tethering-keepalive
    VERSION 0.1
    DESCRIPTION "keepalive plugin for tethering")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(FindPkgConfig)
include(FeatureSummary)
include(GNUInstallDirs)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GSUPPLICANT libgsupplicant REQUIRED IMPORTED_TARGET)
pkg_check_modules(GLIB2 glib-2.0 REQUIRED IMPORTED_TARGET)
pkg_check_modules(CONNMAN connman REQUIRED IMPORTED_TARGET)

set(SRC src/tethering_keepalive_plugin.c)

add_library(sailfish-connman-plugin-tethering-keepalive SHARED ${SRC})
target_link_libraries(sailfish-connman-plugin-tethering-keepalive PUBLIC
    PkgConfig::GLIB2
    PkgConfig::GSUPPLICANT)

install(TARGETS sailfish-connman-plugin-tethering-keepalive LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/connman/plugins)
