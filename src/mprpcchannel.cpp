#include "mprpcchannel.h"

/*
    服务请求的消息格式：header_size(4字节) header_str arg_str
                    header_str:servcie_name method_name arg_size 
*/
void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method,
                              google::protobuf::RpcController *controller,
                              const google::protobuf::Message *request,
                              google::protobuf::Message *response,
                              google::protobuf::Closure *done)
{
    const google::protobuf::ServiceDescriptor* sd = method->service();
    std::string service_name = sd->name();
    std::string method_name = method->name();

    uint32_t arg_size = 0;
    std::string arg_str;
    if(request->SerializeToString(&arg_str))
    {
        arg_size = arg_str.size();
    }
    else
    {
        //序列化失败

        return;
    }

    //定义请求的header
    mprpc::MprpcHeader rpc_header;
    rpc_header.set_service_name(service_name);
    rpc_header.set_method_name(method_name);
    rpc_header.set_args_size(arg_size);

    std::string rpc_header_str;
    uint16_t rpc_header_size = 0;
    if(rpc_header.SerializeToString(&rpc_header_str))
    {
        rpc_header_size = rpc_header_str.size();
    }
    else
    {
        //序列化失败
        return;
    }

    //header_size(4字节) header_str arg_str  都有了 组装请求
    //固定header_size 为4字节 确保解析时始终知道前 4 字节是头部大小
    std::string send_rpc_request;
    //将header_size 转换成网路字节序
    rpc_header_size = htons(rpc_header_size);
    send_rpc_request.insert(0,(char*)rpc_header_size,4);
    send_rpc_request+=rpc_header_str;
    send_rpc_request+=arg_str;

    // 使用tcp编程，完成rpc方法的远程调用
    int clientfd = socket(AF_INET,SOCK_STREAM,0);

    if(-1==clientfd)
    {
        //创建套接字失败
        return;
    }

    //读取配置信息
    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port= atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    //连接Rpc 服务
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htonl(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if(-1 == connect(clientfd,(struct sockaddr*)&server_addr,sizeof(server_addr)))
    {
        //连接失败
        close(clientfd);
        return;
    }
    
    //发送请求
    if(-1== send(clientfd,send_rpc_request.c_str(),send_rpc_request.size(),0))
    {
        //发送请求失败
    }
    //接受响应
    char recv_buf[1024] = {0};
    int recv_size = 0;
    if(-1 == recv(clientfd,recv_buf,1024,0))
    {
        //接收请求失败
        close(clientfd);
        return;
    }

    //反序列化响应
    if(!response->ParseFromArray(recv_buf,recv_size))
    {
        //反序列化失败
        close(clientfd);
        return;
    }
    //关闭连接
    close(clientfd);
    
}