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
    if(args <2)
    {
        ShowArgHelp();
        exit(EXIT_FAILURE);
    }
    int opt = 0;
    std::string config_file;
    while(opt = getopt(args,argv,"i:") != -1)
    {
        switch (opt)
        {
        case 'i':
            config_file = optarg;
            break;
        case '?':
        case ':':
            ShowArgHelp();
            exit(EXIT_FAILURE);
            break;
        }
    }

    //加载配置文件
    m_config.LoadConfigFile(config_file);
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