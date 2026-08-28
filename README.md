# Anclient.cmake
The cmake version of Anclient.

# Test

- Jun 4, 2026

Switch to MinGW with g++ 16.1.0.

Install latest semantier_generator from PYPI.

```
    cd test

    python3 -m semantier_gen settings/semantier-settings.json ast
    python3 -m semantier_gen settings/doclientier-settings.json ast
    python3 -m semantier_gen settings/gen-anclient-settings.json ast
```

- required service [2026/06/13]

1. synode infor-17.1 port 8957

1. jserv-sample JsampleApp, port 8080

1. central registry 0.1.8

    ```
    Synode Semantic-jserv Sample APP
    http://localhost:8080/jserv-sample

    Synode infor-17-1
    http://192.168.0.231:8957/jserv-album

    . . . . . . . . Registry Centre is running . . . . . . . 
    JServ http://192.168.101.5:1991/regist-central
    ```

- Notes

   - To update build/anson.cmake-src only,

     ```
         rm -rf build/Debug/_deps/anson.cmake-src build/Debug/_deps/anson.cmake-build build/Debug/_deps/anson.cmake-subbuild
     ```

     and run cmake.

# Compile on Ubuntu

## Prerequisites

Install build tooling:

```
    sudo apt update
    sudo apt install -y build-essential ninja-build git curl zip unzip tar \
        pkg-config autoconf automake libtool python3
    pip install --user meson
```

`meson` is required because `cpr` (fetched automatically via `FetchContent`) builds
`libcurl` and some of its dependencies with Meson.

CMake v4.4.2 or newer, and `g++-16`.

## vcpkg

```
    cd ..
    git clone https://github.com/microsoft/vcpkg.git
    cd vcpkg
    ./bootstrap-vcpkg.sh
```

Install the dependencies for the Linux triplet:

```
    ./vcpkg install openssl:x64-linux boost-url:x64-linux \
        entt:x64-linux nlohmann-json:x64-linux
```

`CMakeLists.txt` auto-selects the `x64-linux` triplet on Linux, so no extra
`-DVCPKG_TARGET_TRIPLET` flag is needed for a native build. Pass one explicitly
only if you want to cross-compile (e.g. Windows binaries via MinGW).

## Build

Should be built as Qt Project.