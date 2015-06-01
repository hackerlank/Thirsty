# Thirsty

Thirsty is a collection of usefull C++ componet in [folly](https://github.com/facebook/folly)


## Installation

To build the library and test suits:

### Build on Windows (Windows 7 x64)

1. Obtain boost library(http://boost.org) 
2. Set environment variable `BOOST_ROOT` as directory path of boost library
3. Run `build-msvc2013.bat` to generate Visual C++ 2013 solution files

### Build on Linux (Ubuntu 14.04 x64)

1. Obtain boost library(http://boost.org) 
2. Build and install `boost.system`, header files in `/usr/local/include`
3. Run `./build_gmake.sh` to generate make files and build executables
