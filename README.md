# Thirsty

"You must be thirsty."  --- &lt;Life of Pi> movie

Copyright(c) 2014 ichenq@gmail.com

## Introduction

Thirsty is a server framework layer for mobile game server, handle the complexity 
of the C++ world with an emphasis on performance and safety, take a very simplified 
interface for Lua which making game development more enjoyable.

main design goals:

* Linux and Windows TCP and HTTP networking support

* Actor concurrency model  

* Coroutine based asychrounous I/O 

Thirsty is still under heavily development.


## Installation

To build the library and test suits:

### Build on Windows (Windows 7 x64)

1. Obtain boost library(http://boost.org) 
2. Set environment variable `BOOST_ROOT` as directory path of boost library
3. Build `boost.system` `($BOOST_ROOT)/stage/lib`
4. Run `build-msvc2013.bat` to generate Visual C++ 2013 solution files
5. Build the solution files with Visual Studio 2013(or Visual Studio 2013 express)

### Build on Linux (Ubuntu 12.04 x64)

1. Obtain boost library(http://boost.org) 
2. Build and install `boost.system`, header files in `/usr/local/include`
3. Run `./build_gmake.sh` to generate make files and build executables
