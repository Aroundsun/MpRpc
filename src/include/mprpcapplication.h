#pragma once
#include"mprpcconfig.h"
#include<iostream>
#include <unistd.h>
/*
    框架基础类 主要负责框架的初始化
*/

class MprpcApplication
{
public:
    //初始化框架
    static void init(int args,char **argv);
    //获取单例对象
    static  MprpcApplication& GetInstance();
    //获取配置对象
    static MprpcConfig& GetConfig();
private:
    static MprpcConfig m_config;

    MprpcApplication();

    MprpcApplication(const MprpcApplication&) = delete;
    MprpcApplication(MprpcApplication&&)= delete;
};