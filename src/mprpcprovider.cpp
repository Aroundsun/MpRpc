#include "mpcrpcprovider.h"

void MprpcProvider::NotifyService(google::protobuf::Service *service)
{
    // 获取服务对象的描述信息
    const google::protobuf::ServiceDescriptor *service_descriptor = service->GetDescriptor();

    ServiceInfo service_info;
    // 获取服务名字
    std::string service_name = service_descriptor->name();
    // 获取服务方法数
    size_t methodCnt = service_descriptor->method_count();

    LOG_INFO("service_name:%s", service_name.c_str());

    for (int i = 0; i < methodCnt; ++i)
    {
        // 获取服务对象的服务方法描述信息
        const google::protobuf::MethodDescriptor *method_dsscriptor = service_descriptor->method(i);
        std::string method_name = method_dsscriptor->name();
        service_info.m_serviceMethondMap.insert({method_name, method_dsscriptor});

        LOG_INFO("method_name:%s", method_name.c_str());
    }

    service_info.m_service = service;
    m_serviceMap.insert({service_name, service_info});
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
    // 读取配置
    const std::string rpcserverIp = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    const uint16_t rpcserverPort = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    muduo::net::InetAddress address(rpcserverIp, rpcserverPort);

    // 创建TCP Server 对象
    muduo::net::TcpServer server(&m_eventLoop, address, "mprpcprovider");

    // 设置线程数量
    server.setThreadNum(4);
    // 绑定连接回调和消息回调
    server.setConnectionCallback(
        std::bind(&MprpcProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(
        std::bind(&MprpcProvider::OnMessage,
                  this,
                  std::placeholders::_1,
                  std::placeholders::_2,
                  std::placeholders::_3));
    // 把当前rpc节点上要发布的服务全部注册到zk上面，让rpc client可以从zk上发现服务
    // session timeout   30s     zkclient 网络I/O线程  1/3 * timeout 时间发送ping消息
    zkCli zkClient;
    zkClient.Start();
    // service_name为永久性节点    method_name为临时性节点
    for (auto &sp : m_serviceMap)
    {
        // /service_name   /UserServiceRpc
        std::string service_path = "/" + sp.first;
        zkClient.Create(service_path.c_str(), nullptr, 0);
        for (auto &mp : sp.second.m_serviceMethondMap)
        {
            // /service_name/method_name   /UserServiceRpc/Login 存储当前这个rpc服务节点主机的ip和port
            std::string method_path = service_path + "/" + mp.first;
            char method_path_data[128] = {0};
            sprintf(method_path_data, "%s:%d", rpcserverIp .c_str(), rpcserverPort );
            // ZOO_EPHEMERAL表示znode是一个临时性节点
            zkClient.Create(method_path.c_str(), method_path_data, strlen(method_path_data), ZOO_EPHEMERAL);
        }
    }

    LOG_INFO("mprpcserver start server Ip:%s, Port:%d", rpcserverIp.c_str(), rpcserverPort);

    // 启动网络服务
    server.start();
    m_eventLoop.loop();
}
// 连接回调
void MprpcProvider::OnConnection(const muduo::net::TcpConnectionPtr &conn)
{
    if (conn->connected()) // 连接成功 记录日志 记录对端IP：Port
    {
        LOG_INFO("pree{%s} connect OK ", conn->peerAddress().toIpPort().c_str());
    }
    else
    {
        LOG_ERR("pree{%s} connect faild", conn->peerAddress().toIpPort().c_str());

        conn->shutdown();
    }
}

/*
   消息格式： 服务请求的消息格式：header_size(4字节) header_str arg_str
    （header_str:servcie_name method_name arg_size）
*/
// 消息回调
void MprpcProvider::OnMessage(const muduo::net::TcpConnectionPtr &conn,
                              muduo::net::Buffer *buffer,
                              muduo::Timestamp receiveTime)
{

    // 接受Rpc服务请求的字符流
    std::string recv_str = buffer->retrieveAllAsString();
    // 获取header_size (前四个字节)
    uint32_t header_size;
    recv_str.copy((char *)&header_size, 4, 0);

    // 根据header_size 取出header_str
    std::string header_str = recv_str.substr(4, header_size);
    mprpc::MprpcHeader rpcheader;
    std::string service_name;
    std::string method_name;
    uint32_t arg_size;
    if (rpcheader.ParseFromString(header_str)) // 序列化成功
    {
        service_name = rpcheader.service_name();
        method_name = rpcheader.method_name();
        arg_size = rpcheader.args_size();
    }
    else // 序列化失败
    {
        LOG_ERR("rpc_header_str:%s  parse error!!!", header_str.c_str());
        return;
    }
    // 获取参数字符流
    std::string arg_str = recv_str.substr(4 + header_size, arg_size);

    // 获取service 对象 和method 对象
    auto it = m_serviceMap.find(service_name);
    if (it == m_serviceMap.end())
    {
        LOG_ERR("service_name:%s  is not exist", service_name.c_str());
        return;
    }
    auto mit = it->second.m_serviceMethondMap.find(method_name);
    if (mit == it->second.m_serviceMethondMap.end())
    {
        LOG_ERR("method_name:%s  is not exist", method_name.c_str());

        return;
    }
    // 获取服务对象
    google::protobuf::Service *service = it->second.m_service;
    // 获取服务方法
    const google::protobuf::MethodDescriptor *methodDec = mit->second;

    // 动态创建Rpc 请求对象request
    google::protobuf::Message *request = service->GetRequestPrototype(methodDec).New();
    if (!request->ParseFromString(arg_str)) // 请求解析失败
    {
        LOG_ERR("request qarse error request_str:%s", arg_str.c_str());
        return;
    }

    google::protobuf::Message *response = service->GetResponsePrototype(methodDec).New();

    // 绑定响应回调
    google::protobuf::Closure *done = google::protobuf::NewCallback<MprpcProvider, const muduo::net::TcpConnectionPtr &,
                                                                    google::protobuf::Message *>(this, &MprpcProvider::SendRpcResponse,
                                                                                                 conn, response);

    service->CallMethod(methodDec, nullptr, request, response, done);
}

// Closure的回调操作，用于序列化rpc的响应和网络发送
void MprpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr &conn, google::protobuf::Message *respones)
{
    std::string respones_str;
    if (respones->SerializeToString(&respones_str)) // 序列化成功
    {
        conn->send(respones_str);
    }
    else // 序列化失败
    {
        LOG_ERR("serialize response_str error!");
        return;
    }
    conn->shutdown();
}