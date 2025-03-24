#aux_source_directory(. SRC_LIST)
set(SRC_LIST 
    mprpcapplication.cpp
    mprpcconfig.cpp 
    mprpcheader.pb.cc
    mprpcprovider.cpp 
    mprpcchannel.cpp
    mprpccontroller.cpp
    logger.cpp
    zookeeperuilt.cpp)
add_library(mprpc ${SRC_LIST})
target_link_libraries(mprpc muduo_net muduo_base pthread zookeeper_mt)