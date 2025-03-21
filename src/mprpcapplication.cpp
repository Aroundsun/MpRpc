#include"mprpcapplication.h"

//定义静态配置对象
MprpcConfig MprpcApplication::m_config;

//参数提示
void ShowArgHelp()
{
    std::cout<<"format: command -i <configfile>" << std::endl;
}
//初始化框架
void MprpcApplication::init(int args,char **argv)
{

    
}
//获取单例对象
MprpcApplication& MprpcApplication::GetInstance()
{
    MprpcApplication mprpcapp;
    return mprpcapp;
}
//获取配置对象
MprpcConfig& MprpcApplication::GetConfig()
{
    return m_config;
}