#pragma once
/*
实现连接池功能模块
*/
#include<string>
#include<queue>
#include<mutex>
#include"connection.h"
#include"public.h"
#include<atomic>
#include<thread>
#include<memory>
#include<functional>
#include<condition_variable>
#include"connection.h"
using namespace std;

class connectionpool
{
public:
	static connectionpool* getConnectionpool();
	//外部提供一个接口，方便外部获得空闲连接
	shared_ptr<connection>getConnection();
private:
	connectionpool();


	bool loadConfigFile();

	//运行在独立的线程中，专门负责生产新连接
	void productConnectionTask();
	//扫描maxIdleTime时间的空闲连接，进行对于的来连接回收
	void scannerConnectionTask();

	string ip;
	unsigned short port;
	string username;
	string password;
	string dbname;
	int initSize;//连接池初始连接量
	int maxSize;//连接池最大连接量
	int maxIdleTime;//连接池最大空闲时间
	int connectionTimeout;//连接池获取连接超时时间

	queue<connection*> connectionQue;
	mutex queueMutex;//维护队列的线程安全互斥锁
	atomic_int connectioncnt;//记录连接数量
	condition_variable cv;//设置条件变量，用于进程通信
};