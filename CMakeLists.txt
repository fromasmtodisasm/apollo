# Part of the apollo library -- Copyright (c) Christian Neumüller 2015
# This file is subject to the terms of the BSD 2-Clause License.
# See LICENSE.txt or http://opensource.org/licenses/BSD-2-Clause

cmake_minimum_required(VERSION 2.8)
project(apollo)

# http://stackoverflow.com/a/13104057/2128694
macro(prepend_to_all _srcs _path)
    unset(_tmp)
    foreach(src ${${_srcs}})
        list(APPEND _tmp ${_path}${src})
    endforeach()
    set(${_srcs} ${_tmp})
endmacro()

macro(needs_apollo_dll _target)
    if (BUILD_SHARED_LIBS AND MSVC)
        add_custom_command(TARGET ${_target} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:apollo>" "$<TARGET_FILE_DIR:${_target}>"
            COMMENT "Copying apollo DLL to ${_target} directory...")
    endif()
endmacro()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(CTest)

set(CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/" ${CMAKE_MODULE_PATH})


if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    option(BUILD_SHARED_LIBS "Build apollo as a dynamic library (DLL/.so)." OFF)
    set(APOLLO_LUA_VERSION "" CACHE STRING
        "Set to a dotted Lua version number, e.g.: 5.1.5, 5.3.0. Empty: Newest available.")
endif()

if (MSVC)
    add_definitions("/W4" "/MP")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    if (BUILD_SHARED_LIBS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
    endif()
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set (_cfgfile
            "${CMAKE_CURRENT_SOURCE_DIR}/include/apollo/detail/clang_stdlib_config.hpp")
        add_definitions("-DBOOST_STDLIB_CONFIG=\"${_cfgfile}\"")
        add_definitions("-Weverything")
        add_definitions("-Wno-c++98-compat -Wno-c++98-compat-pedantic")
        add_definitions("-Wno-weak-vtables -Wno-padded")
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        add_definitions("-Wall -Wextra -pedantic")
        add_definitions("-Winit-self -Woverloaded-virtual")
        add_definitions("-Wlogical-op")
        add_definitions("-Wmissing-declarations")
        add_definitions("-Wsign-conversion -Wsign-promo -Wconversion")
        add_definitions("-Wcast-align -Wcast-qual -Wold-style-cast")
        add_definitions("-Wstrict-overflow=5")
        add_definitions("-Wstrict-null-sentinel")
        add_definitions("-Wformat=2")
        add_definitions("-Wmissing-include-dirs")
        add_definitions("-Wdisabled-optimization")
        add_definitions("-Wredundant-decls")
        add_definitions("-Wshadow")
        add_definitions("-Wundef")
        add_definitions("-Wswitch-default")
        add_definitions("-Wnoexcept")
    endif()
endif()

set(APOLLO_DYNAMIC_LINK ${BUILD_SHARED_LIBS})

# Configure the build info header
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include/apollo")
set(APOLLO_BUILDINFO_HPP
    "${CMAKE_CURRENT_BINARY_DIR}/include/apollo/build_information.hpp")
configure_file(build_information.hpp.cmake_in ${APOLLO_BUILDINFO_HPP})


include_directories(BEFORE
    "src"
    "${CMAKE_CURRENT_BINARY_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(Boost_USE_STATIC_LIBS     ON)
set(Boost_USE_MULTITHREADED   ON)
set(Boost_USE_STATIC_RUNTIME OFF)

set(Boost_FIND_COMPONENTS "")
if (BUILD_TESTING)
    list(APPEND Boost_FIND_COMPONENTS unit_test_framework)
endif()

find_package(Boost 1.53 REQUIRED ${Boost_FIND_COMPONENTS})

add_definitions("-DBOOST_ALL_NO_LIB")

if (APOLLO_LUA_VERSION)
    find_package(Lua ${APOLLO_LUA_VERSION} EXACT REQUIRED)
else()
    find_package(Lua REQUIRED)
endif()

set(LUA_INCLUDE_DIRS "${LUA_INCLUDE_DIR}")

include_directories(
    ${Boost_INCLUDE_DIRS} ${LUA_INCLUDE_DIR})

add_subdirectory("src")
add_subdirectory("doc")
if (BUILD_TESTING)
    add_subdirectory("test")
endif()

install(FILES "cmake/Modules/FindApollo.cmake" DESTINATION CMake/Modules)
install(FILES "cmake/Modules/FindLua.cmake" DESTINATION CMake/Modules)
install(FILES ${APOLLO_BUILDINFO_HPP} DESTINATION "include/apollo")
