#pragma once
#include <google/protobuf/service.h>
#include<google/protobuf/descriptor.h>
#include<google/protobuf/message.h>
#include<string>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include"logger.h"
#include"mprpcheader.pb.h"
#include"mprpcconfig.h"
#include"mprpcapplication.h"
class MprpcChannel : public google::protobuf::RpcChannel
{
public:
    //rpc方法调用的数据数据序列化和网络发送 
    void CallMethod(const google::protobuf::MethodDescriptor *method,
                    google::protobuf::RpcController *controller,
                    const google::protobuf::Message *request,
                    google::protobuf::Message *response,
                    google::protobuf::Closure *done);
};