#pragma once
#include"mpcrpcprovider.h"


void MprpcProvider::NotifyService(google::protobuf::Service* service)
{
    //获取服务对象的描述信息
    const google::protobuf::ServiceDescriptor* service_descriptor = service->GetDescriptor();

    ServiceInfo service_info;
    //获取服务名字
    std::string service_name = service_descriptor->name();
    //获取服务方法数
    size_t methodCnt = service_descriptor->method_count();

    LOG_INFO("service_name:%s",service_name.c_str());

    for(int i = 0;i<methodCnt;++i)      
    {
        //获取服务对象的服务方法描述信息
        const google::protobuf::MethodDescriptor* method_dsscriptor = service_descriptor->method(i);
        std::string method_name = method_dsscriptor->name();
        service_info.m_serviceMethondMap.insert({method_name,method_dsscriptor});

        LOG_INFO("method_name:%s", method_name.c_str());

    }
    service_info.m_service = service;
    m_serviceMap.insert({service_name,service_info});
}

/*
    # rpc节点的ip地址
    rpcserverip=127.0.0.1     
    # rpc节点的port端口号
    rpcserverport=8000
    # zk的ip地址
    zookeeperip=127.0.0.1
    # zk的port端口号
    zookeeperport=2181

*/
void MprpcProvider::Run()
{
    //读取配置
    const std::string rpcserverIp = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    const uint16_t rpcserverPort = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    muduo::net::InetAddress address(rpcserverIp,rpcserverPort);

    //创建TCP Server 对象
    muduo::net::TcpServer server(&m_eventLoop,address,"mprpcprovider");

    //设置线程数量
    server.setThreadNum(4);
    //绑定连接回调和消息回调
    server.setConnectionCallback(
                std::bind(&MprpcProvider::OnConnection,this,std::placeholders::_1)  
    );
    server.setMessageCallback(
                std::bind(MprpcProvider::OnMessage,
                            this,
                            std::placeholders::_1, 
                            std::placeholders::_2, 
                            std::placeholders::_3) 
    );

                                    
    LOG_INFO("mprpcserver start server Ip:%s, Port:%d",rpcserverIp,rpcserverPort);

    //启动网络服务
    server.start();
    m_eventLoop.loop();
}
//连接回调
void MprpcProvider::OnConnection(muduo::net::TcpConnectionPtr& conn)
{

}
//消息回调
void MprpcProvider::OnMessage(muduo::net::TcpConnectionPtr& conn,google::protobuf::Message*msg)
{

}