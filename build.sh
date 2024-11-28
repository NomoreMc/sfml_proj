#! /bin/bash

PROJ_ROOT_PATH=`pwd`

clean() {
    cd ${PROJ_ROOT_PATH}
    rm -rf ${PROJ_ROOT_PATH}/build
    rm -rf ${PROJ_ROOT_PATH}/output
    rm -rf ${PROJ_ROOT_PATH}/report
}

build() {
    cd ${PROJ_ROOT_PATH}
    mkdir -p ${PROJ_ROOT_PATH}/build
    cd ${PROJ_ROOT_PATH}/build
    cmake ..
    make -j4
}

clean
build