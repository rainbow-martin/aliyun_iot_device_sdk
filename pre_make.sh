#! /bin/bash

CROSS_COMPILE_PATH="/home/martin/myworks/raspberry/tools-master/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/"
CXX=$CROSS_COMPILE_PATH"arm-linux-gnueabihf-g++"
CC=$CROSS_COMPILE_PATH"arm-linux-gnueabihf-gcc"
AR=$CROSS_COMPILE_PATH"arm-linux-gnueabihf-ar"
export CROSS_COMPILE_PATH
export CXX
export CC
export AR
