#! /bin/bash

PROJ_ROOT_PATH=`pwd`

# 删除所有的构建中间文件
run() {
    cd ${PROJ_ROOT_PATH}/output/bin
    ./sfml_demo
}

run