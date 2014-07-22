# Thirsty

"You must be thirsty."  --- &lt;Life of Pi> movie

Copyright(c) 2014 ichenq@gmail.com

## Introduction

Thirsty is a server framework layer for mobile game server, handle the complexity 
of the C++ world with an emphasis on performance and safety, take a very simplified 
interface for Lua which making game development more enjoyable.

main features are shown as follow:

* Cross-platform networking support

* Multi-threaded model with message queue


## Installation

To build the library and tests:

### Build on Windows

1. Obtain boost library(http://boost.org) 
2. Set environment variable `BOOST_ROOT` as directory path of boost library
3. Build `boost.system`, `boost.date_time` and `boost.thread` in `($BOOST_ROOT)/stage/lib-x64`
4. Run `build-msvc2013.bat` to generate Visual C++ 2013 solution files
5. Open the solution file with Visual Studio 2013(or Visual Studio 2013 express) to build

### Build on Linux

1. Obtain boost library(http://boost.org) 
2. Build and install `boost.system`, `boost.date_time` and `boost.thread`, header files in `/usr/local/include`
3. Run `./build_gmake.sh` to generate make files
4. Run `make -f Thirsty.make config=release32`
