#!/bin/sh

./premake4 --file=thirsty.lua gmake
make config=release64 clean
make config=release64 

./premake4 --file=unittest.lua gmake
make config=release64 clean
make config=release64 

