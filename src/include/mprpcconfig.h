#pragma once 
#include<string>
#include<unordered_map>
#include<fstream>
#include"logger.h"
/*
    配置读取类
*/
class MprpcConfig
{
public:
    //负责加载解析配置文件
    void LoadConfigFile(const std::string& config_file); 
    //获取配置项信息
    std::string Load(const std::string &key);
private:
    //存储配置信息
    std::unordered_map<std::string,std::string> m_configMap;
    //去掉字符串前后的空格
    static void Trim(std::string &src_buf);
};