#include<muduo/net/TcpServer.h>
#include<muduo/net/TcpConnection.h>
#include<muduo/net/EventLoop.h>

#include<google/protobuf/service.h>
#include<google/protobuf/descriptor.h>
#include<functional>
#include<string>
#include<unordered_map>

#include"logger.h"
#include"mprpcapplication.h"

//发布Rpc 服务的网络对象类
class MprpcProvider
{
public:
    MprpcProvider(){};
    ~MprpcProvider(){};
    MprpcProvider(const MprpcProvider&) = delete;
    MprpcProvider(MprpcProvider&&) = delete;
    MprpcProvider& operator=(const MprpcProvider&) = delete;
    MprpcProvider& operator=(MprpcProvider&&) = delete;

    //为外部发布服务对象
    void NotifyService(google::protobuf::Service* );

    //启动服务
    void Run();
private:
    //服务信息
    struct ServiceInfo
    {
        //服务对象
        google::protobuf::Service* m_service;
        //服务对象所对应的服务方法 
        std::unordered_map<std::string,const google::protobuf::MethodDescriptor*> m_serviceMethondMap;
    };
    //连接回调
    void OnConnection(muduo::net::TcpConnectionPtr&);
    //消息回调
    void OnMessage(muduo::net::TcpConnectionPtr&,google::protobuf::Message*);

private:
    muduo::net::EventLoop m_eventLoop;
    std::unordered_map<std::string,ServiceInfo> m_serviceMap;

};