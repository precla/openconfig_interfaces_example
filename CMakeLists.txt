cmake_minimum_required(VERSION 3.21)
project(interfaces VERSION 0.6.3)

include(CompileOptions.cmake)

set(PLUGIN 0 CACHE BOOL "Build a plugin")
option(INTERFACES_PLUGIN "Enable interfaces plugin" ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules")
find_package(LIBYANG REQUIRED)
find_package(SYSREPO REQUIRED)

if(INTERFACES_PLUGIN)
    add_subdirectory(src/interfaces)
endif()
