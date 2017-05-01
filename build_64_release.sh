#!/bin/bash

PATH="/home/vasek/Programs/CMake.3.8.0/bin/:$PATH"
echo ${PATH}
PROJECT_PATH=${PWD}/
BUILD_PATH=${PWD}/build/Linux64_Release

rm -rf ${BUILD_PATH}
mkdir ${BUILD_PATH}
cd ${BUILD_PATH}

cmake -G"CodeBlocks - Unix Makefiles" -DCMAKE_BUILD_TYPE=Release DCMAKE_CXX_COMPILER:STRING=/usr/bin/g++ -DCMAKE_CXX_FLAGS:STRING=-m64 ${PROJECT_PATH}
cmake --build . --target all

${BUILD_PATH}/CMakeCrossTest
