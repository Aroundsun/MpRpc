#pragma once 
#include<string>
#include<unordered_map>
/*
    配置读取类
*/
class mprpcConfig
{
public:
    //负责加载解析配置文件
    void LoadConfigFile(const std::string& config_file); 
    //获取配置项信息
    std::string Load(const std::string &key);
private:
    //存储配置信息
    std::unordered_map<std::string,std::string> m_configMap;
    
};