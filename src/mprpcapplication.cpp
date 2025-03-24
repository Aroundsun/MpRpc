#include "mprpcapplication.h"

// 定义静态配置对象
MprpcConfig MprpcApplication::m_config;

// 参数提示
void ShowArgHelp()
{
    std::cout << "format: command -i <configfile>" << std::endl;
}
// 初始化框架
void MprpcApplication::init(int args, char **argv)
{
    if (args < 2)
    {
        ShowArgHelp();
        std::cout << "1";

        exit(EXIT_FAILURE);
    }
    int c = 0;
    std::string config_file;
    while ((c = getopt(args, argv, "i:")) != -1)
    {

        switch (c)
        {
        case 'i':
            config_file = optarg;
            break;
        case '?':
            ShowArgHelp();
            exit(EXIT_FAILURE);
        case ':':
            ShowArgHelp();
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }
    if (config_file.empty())
    {
        std::cout << argv[0] << std::endl;
        std::cout << argv[1] << std::endl;
        std::cout << argv[2] << std::endl;
    }

    // 加载配置文件
    m_config.LoadConfigFile(config_file);
}
// 获取单例对象
MprpcApplication &MprpcApplication::GetInstance()
{
    static MprpcApplication mprpcapp;
    return mprpcapp;
}
// 获取配置对象
MprpcConfig &MprpcApplication::GetConfig()
{
    return m_config;
}