#include"mprpcconfig.h"
#include<iostream>
void MprpcConfig::LoadConfigFile(const std::string& config_file)
{


    std::fstream file(config_file);

    std::string line;
    if(!file)
    {
        //打开文件失败,记录日志
        LOG_ERR("file:%s open faild",config_file.c_str());
        exit(EXIT_FAILURE);
    }
    int colNum = 0;
    while(std::getline(file,line))
    {
        ++colNum;//记录行号
        Trim(line);
        if(line[0] == '#' || line.empty())
        {
            continue;
        }
        //解析配置项
        int idx = line.find('=');
        if(idx == -1)
        {
            //解析文件出错
            LOG_ERR("file:%s,col:%d faild",config_file.c_str(),colNum);
            exit(EXIT_FAILURE);
        }

        std::string key;
        std::string value;
        key = line.substr(0,idx);
        Trim(key);
        int endIdx = line.find('\n',idx);
        value = line.substr(idx+1,endIdx-idx-1);
        Trim(value);
        m_configMap.insert({key,value});
    }

    file.close();

}

std::string MprpcConfig::Load(const std::string& key)
{
    auto it = m_configMap.find(key);
    if (it == m_configMap.end())
    {
        return "";
    }
    return it->second;
}

// 去掉字符串前后的空格
void MprpcConfig::Trim(std::string &src_buf)
{
    int idx = src_buf.find_first_not_of(' ');
    if (idx != -1)
    {
        // 说明字符串前面有空格
        src_buf = src_buf.substr(idx, src_buf.size()-idx);
    }
    // 去掉字符串后面多余的空格
    idx = src_buf.find_last_not_of(' ');
    if (idx != -1)
    {
        // 说明字符串后面有空格
        src_buf = src_buf.substr(0, idx+1);
    }
}