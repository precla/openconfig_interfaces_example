project(interfaces)

set(SOURCES
    plugin/subscription/change.c
    plugin.c)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules")
find_package(LIBYANG REQUIRED)
find_package(SYSREPO REQUIRED)

if(PLUGIN)
    add_library(${PROJECT_NAME} MODULE ${SOURCES})
    install(TARGETS ${PROJECT_NAME} DESTINATION ${SRPD_PLUGINS_PATH})
else()
    set(SOURCES ${SOURCES} main.c)
    add_executable(${PROJECT_NAME} ${SOURCES})
    install(TARGETS ${PROJECT_NAME} DESTINATION bin)
endif()

target_link_libraries(
    ${PROJECT_NAME}
    ${SYSREPO_LIBRARIES}
    ${LIBYANG_LIBRARIES})

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC ${SYSREPO_INCLUDE_DIRS}
    PUBLIC ${LIBYANG_INCLUDE_DIRS}
    PUBLIC ${CMAKE_SOURCE_DIR}/src/interfaces
    PUBLIC ${CMAKE_SOURCE_DIR}/src/utils
    PRIVATE ${CMAKE_SOURCE_DIR}/deps)
