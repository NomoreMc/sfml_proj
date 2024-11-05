#! /bin/bash

PROJ_ROOT_PATH=`pwd`

build() {
    cd ${PROJ_ROOT_PATH}
    mkdir -p ${PROJ_ROOT_PATH}/build
    cd ${PROJ_ROOT_PATH}/build
    cmake ..
    make -j4
}

build