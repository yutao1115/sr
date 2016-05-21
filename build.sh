#!/bin/bash
rm -rf ./lib/*
mkdir ./lib

cd dep/googlemock
mkdir winbuild
rm -rf winbuild/*
rm -rf gtest/*
cd winbuild
cmake ..
make clean
make
cp gtest/*.a ../../../lib
cp *.a       ../../../lib

cd ../../..
mkdir winbuild
rm -rf winbuild/*
cd winbuild
cmake -DCMAKE_BUILD_TYPE=Debug    -G"Unix Makefiles" ..
make clean
make
cd ..
