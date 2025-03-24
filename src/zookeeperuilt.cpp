#include "zookeeperuilt.h"
#include <zookeeper/zookeeper.h>
// 全局的watcher观察器   zkserver给zkclient的通知
void global_watcher(zhandle_t *zh, int type,
                    int state, const char *path, void *watcherCtx)
{
    if (type == ZOO_SESSION_EVENT) // 监听回话事件
    {
        if (state == ZOO_CONNECTED_STATE) // zkclient和zkserver连接成功
        {
            sem_t *sem = (sem_t *)zoo_get_context(zh); // 获取信号量
            sem_post(sem);                             // 释放信号量
        }
    }
}

zkCli::zkCli() : m_zhandle(nullptr)
{
}

zkCli::~zkCli()
{
    if (nullptr != m_zhandle)
    {
        zookeeper_close(m_zhandle);
    }
}

// zkclient启动连接zkserver
void zkCli::Start()
{
    // 读取配置

    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("zookeeperip");
    std::string port = MprpcApplication::GetInstance().GetConfig().Load("zookeeperport");
    std::string host = ip + ":" + port;

    // 初始化 ZooKeeper 客户端，建立与 ZooKeeper 服务器的连接
    m_zhandle = zookeeper_init(host.c_str(), global_watcher, 3000, nullptr, nullptr, 0);
    if (m_zhandle == nullptr) // 连接初始化失败
    {
        LOG_ERR("zookeeper_init error");
        exit(EXIT_FAILURE);
    }
    // 使用信号量等待连接成功
    sem_t sem;
    sem_init(&sem, 0, 0);
    // 将信号量和句柄关联
    zoo_set_context(m_zhandle, &sem);

    sem_wait(&sem);
    LOG_INFO("zookeeper_init success!");
}

// 在zkserver上根据指定的path创建znode节点
void zkCli::Create(const char *path, const char *data, int datalen, int state)
{
    char path_buffer[128];               // 用于存储创建的 ZNode 实际路径
    int bufferlen = sizeof(path_buffer); // 路径缓冲区大小
    int flag;

    // 先判断 path 表示的 ZNode 节点是否存在，如果存在，就不再重复创建
    flag = zoo_exists(m_zhandle, path, 0, nullptr);

    if (ZNONODE == flag) // ZNONODE 表示目标路径的 ZNode 不存在
    {
        // 创建指定 path 的 ZNode 节点
        flag = zoo_create(m_zhandle, path, data, datalen,
                          &ZOO_OPEN_ACL_UNSAFE, // 访问权限：ZOO_OPEN_ACL_UNSAFE 允许任何人访问
                          state,                // ZNode 类型（持久/临时/顺序）
                          path_buffer,          // 存储创建后的路径
                          bufferlen);           // 路径缓冲区长度

        if (flag == ZOK) // ZOK 表示创建成功
        {
            std::cout << "ZNode 创建成功，路径: " << path << std::endl;
            LOG_ERR("znode create success path: %s",path);
        }
        else // 创建失败
        {
            LOG_ERR("Erroe Code: %d znode create error path: %s",flag,path);
            exit(EXIT_FAILURE); // 终止程序
        }
    }
}
// 根据参数指定的znode节点路径，或者znode节点的值
std::string zkCli::GetData(const char *path)
{
    char buffer[64];
	int bufferlen = sizeof(buffer);
	int flag = zoo_get(m_zhandle, path, 0, buffer, &bufferlen, nullptr);
	if (flag != ZOK)
	{
        LOG_ERR("get znode error... path: %s",path);
		return "";
	}
	else
	{
		return buffer;
	}
}