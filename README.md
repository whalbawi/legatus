# Axle
Axle is a C++ project written in C++20.

## Prerequisites
The project is built using [CMake](https://cmake.org/) and requires version 3.20 or newer to work correctly.

Axle takes a dependency on [googletest](https://github.com/google/googletest) for unit tests and is included
in the source tree as a `git` submodule. Run the following to check out the module and make it available
to the build system:
```bash
$ cd <path-to-axle>
$ git submodule update --init
```

## Building
Axle is tested on Linux and macOS.

The following will generate the build system and then build all targets:
```bash
$ cd <path-to-axle>
$ cmake -B build
$ cmake --build build
```

## Tests
Start by building the unit tests executable:
```bash
$ cmake --build build --target axle-tests
```

Now run the unit tests using CMake's `ctest` utility:
```bash
$ ctest --test-dir build -R unit-tests -V
```

Alternatively, run the tests executable directly:
```bash
$ ./build/axle-tests
```
