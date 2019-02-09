# sbwrp

*sbwrp = SDL2 bgfx wrapper*

This project aims to give you a head start in integrating SDL2 with bgfx.

## Supported platforms by cmake

At the moment the cmake setup is supporting the following platforms

* linux

## Prerequisites

If you use cmake to build this project, you need at least version 3.6.

Compiler has to be

* Clang 3.3 and above
* GCC 5 and above
* vs2017 and above

## Project setup

Run `git submodule update --init --recursive`.

## How to build with cmake

```shell
mkdir tmp
cd tmp
cmake -DCMAKE_BUILD_TYPE=Debug .. # or cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

## How to build without cmake

1. Go into `libs/bgfx` and build bgfx. [See also bgfx documentation about building](https://bkaradzic.github.io/bgfx/build.html)
2. Build SDL2 (in `libs/SDL2-2.0.9`) or get a already compiled version. [See also SDL documentation](https://wiki.libsdl.org/Installation)
3. Build this project files and link the libraries accordingly.

## Contribute

Feel free to create issues and pull requests.

## Todo

* Support Windows cmake build
* Support OSX cmake build

## License

This project is MIT licensed. For more information look into LICENSE.

The libraries in the `libs` folder have their own licenses.
