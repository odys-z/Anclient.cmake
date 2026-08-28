cmake_minimum_required(VERSION 3.30)

# Forces gcc/g++ for sub-builds (works around meson picking up MSVC for libpsl
# on Windows). Harmless on Linux/macOS where these are already the default.
set(ENV{CC} "gcc")
set(ENV{CXX} "g++")
set(CPR_CURL_USE_LIBPSL OFF CACHE BOOL "" FORCE)

# vcpkg must be checked out as a sibling directory: ../vcpkg. See README.
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/../vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
set(VCPKG_INSTALLED_DIR "${CMAKE_SOURCE_DIR}/../vcpkg/installed" CACHE PATH "vcpkg install dir")

# Pick a sane default triplet per-platform. Override on the command line with
# -DVCPKG_TARGET_TRIPLET=... / -DVCPKG_HOST_TRIPLET=... if you need something else
# (e.g. cross-compiling Windows binaries from Linux via MinGW).
if(NOT DEFINED VCPKG_TARGET_TRIPLET)
    if(WIN32)
        set(VCPKG_TARGET_TRIPLET "x64-mingw-dynamic" CACHE STRING "")
    elseif(APPLE)
        set(VCPKG_TARGET_TRIPLET "x64-osx" CACHE STRING "")
    else()
        set(VCPKG_TARGET_TRIPLET "x64-linux" CACHE STRING "")
    endif()
endif()

if(NOT DEFINED VCPKG_HOST_TRIPLET)
    set(VCPKG_HOST_TRIPLET "${VCPKG_TARGET_TRIPLET}" CACHE STRING "")
endif()

project(anclient.cmake
    VERSION 0.1.0
    DESCRIPTION "The cpp peer of Anclient.java"
    HOMEPAGE_URL "https://github.com/odys-z/Anclient.cmake"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

# Generate compile_commands.json for clangd/IDE integration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# vcpkg install openssl
find_package(OpenSSL REQUIRED)

# vcpkg install boost-url
find_package(boost_url CONFIG REQUIRED)

# vcpkg install entt
find_package(EnTT CONFIG REQUIRED)

# vcpkg install nlohmann-json
find_package(nlohmann_json CONFIG REQUIRED)

include(FetchContent)

FetchContent_Declare(
  anson.cmake
  GIT_REPOSITORY https://github.com/odys-z/anson.cmake.git
  GIT_TAG        main
)

set(ANSON_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(anson.cmake)

# Requires: pip install meson (cpr builds libcurl via meson)
FetchContent_Declare(
  cpr
  GIT_REPOSITORY https://github.com/libcpr/cpr.git
  GIT_TAG f091b2c061b307ee89b164c39976fc9202a1c79d) # 1.14.2

FetchContent_MakeAvailable(cpr)

if(WIN32)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        add_compile_options(-Wa,-mbig-obj)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_compile_options(-Xclang -mbig-obj)
    endif()
endif()

option(ANCLIENT_BUILD_TESTS "Build tests for Anclient.cmake" ON)
add_subdirectory(src)

if (ANCLIENT_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
