#!/bin/sh

./premake4 gmake
make -f Thirsty.make config=release64 clean
make -f UnitTest.make config=release64 clean
make -f Thirsty.make config=release64
make -f UnitTest.make config=release64
