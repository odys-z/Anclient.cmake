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
    python3 -m semantier_gen settings/test-settings.json ast
```
