cmake_minimum_required(VERSION 3.30)

# in my/github/vcpkg
# ./vcpkg install entt:x64-windows --x-install-root="../anclient/examples/example.qml/vcpkg_packages"
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/../vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
set(VCPKG_INSTALLED_DIR "${CMAKE_SOURCE_DIR}/../vcpkg/installed" CACHE PATH "vcpkg install dir")

project(anclient.cmake
    VERSION 0.1.0
    DESCRIPTION "The cpp peer of Anclient.java"
    HOMEPAGE_URL "https://github.com/odys-z/Anclient.cmake"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Generate compile_commands.json for clangd/IDE integration
# we have network problem
# This will save time for each clean build.
# [try] Uncomment this for the first time as the network condition is complicate in China, by the GFW.
# qt creator -DQT_CREATOR_SKIP_VCPKG_SETUP:BOOL=ON

# see ping.cpp
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message(STATUS "=========================================== CMAKE_TOOLCHAIN_FILE = ${CMAKE_TOOLCHAIN_FILE}")
message(STATUS "=========================================== CMAKE_CURRENT_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR}")

# ./vcpkg install entt:x64-windows --x-install-root="../Anclient.cmake/vcpkg_packages"
find_package(EnTT CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

include(FetchContent)

FetchContent_Declare(
  anson.cmake
  GIT_REPOSITORY https://github.com/odys-z/anson.cmake.git
  # SOURCE_SUBDIR  antson.cmake
  GIT_TAG        main
)

set(ANSON_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(anson.cmake)

# pip install meson ninja
FetchContent_Declare(
  cpr
  GIT_REPOSITORY https://github.com/libcpr/cpr.git
  GIT_TAG f091b2c061b307ee89b164c39976fc9202a1c79d) # 1.14.2 @ https://github.com/libcpr/cpr/releases

FetchContent_MakeAvailable(cpr)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.15.2
)
FetchContent_MakeAvailable(googletest)

# before src
option(ANCLIENT_BUILD_TESTS "Build tests for Anclient.cmake" ON)
add_subdirectory(src)

# 3. Add tests (only if this is the main project)
if (ANCLIENT_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
