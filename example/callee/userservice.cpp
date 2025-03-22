#include<iostream>
#include<string>
#include"mprpcapplication.h"
#include"mpcrpcprovider.h"
#include"user.pb.h"
//提供服务
class UserService: public: example::UserServiceRpc
{
public:
    void Login(std::string name,std::string pwd)
    {
        std::cout<<"name:"<<name<<" pwd:"<<pwd<<std::endl;
    }
};

int main(int args,char **argv)
{
    //初始化框架
    MprpcApplication::init(args,argv);
    //注册发布服务服务对象
    MprpcProvider provider;
    provider.NotifyService(new UserService());
    //启动服务

    return 0;
}