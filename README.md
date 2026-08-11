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

1. Notes

   - To update build/anson.cmake-src only,

     ```
         rm -rf build/Debug/_deps/anson.cmake-src build/Debug/_deps/anson.cmake-build build/Debug/_deps/anson.cmake-subbuild
     ```

     and run cmake.
