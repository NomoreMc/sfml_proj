#! /bin/bash

PROJ_ROOT_PATH=`pwd`

# 删除所有的构建中间文件
clean() {
    cd ${PROJ_ROOT_PATH}
    rm -rf ${PROJ_ROOT_PATH}/build
    rm -rf ${PROJ_ROOT_PATH}/output
    rm -rf ${PROJ_ROOT_PATH}/report
}

clean