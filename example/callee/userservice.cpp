#include <iostream>
#include <string>
#include "mprpcapplication.h"
#include "mpcrpcprovider.h"
#include "user.pb.h"
// 提供服务
class UserService : public example::UserServiceRpc
{
public:
    bool Login(std::string name, std::string pwd)
    {
        std::cout << "name:" << name << " pwd:" << pwd << std::endl;
        return true;
    }
    void Login(google::protobuf::RpcController *controller,
               const ::example::LoginRequest *request,
               ::example::LoginResponse *response,
               ::google::protobuf::Closure *done)
    {
        // 框架给业务上报了请求参数LoginRequest，应用获取相应数据做本地业务
        std::string name = request->name();
        std::string pwd = request->pwd();

        // 做本地业务
        std::cout<<"本地业务开始......."<<std::endl;
        bool login_result = Login(name, pwd); 
        std::cout<<"本地业务执行结束..."<<std::endl;
        // 把响应写入  包括错误码、错误消息、返回值
        example::ResultCode *code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_sucess(login_result);

        // 执行回调操作   执行响应对象数据的序列化和网络发送（都是由框架来完成的）
        done->Run();
    }
};

int main(int args, char **argv)
{
    // 初始化框架
    MprpcApplication::init(args, argv);

    // 注册发布服务服务对象
    MprpcProvider provider;
    provider.NotifyService(new UserService());
    // 启动服务
    provider.Run();
    return 0;
}