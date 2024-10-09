#include"pch.h"
#include"commonConnectiontionpool.h"
#include<iostream>
using namespace std;

//线程安全的懒汉单例模式
connectionpool* connectionpool::getConnectionpool()
{
	static connectionpool pool;
	return &pool;
}

//从配置文件中加载配置项
bool connectionpool::loadConfigFile()
{
	FILE* pf = fopen("mysqlconnect.ini", "r");
	if (pf == nullptr)
	{
		LOG("mysql.ini file is not exist!");
		return false;
	}

	while (!feof(pf))
	{
		char line[1024] = { 0 };
		fgets(line, 1024, pf);
		string str = line;
		int idx = str.find('=', 0);
		if (idx == -1) // 无效的配置项
		{
			continue;
		}

		// password=123456\n
		int endidx = str.find('\n', idx);
		string key = str.substr(0, idx);
		string value = str.substr(idx + 1, endidx - idx - 1);

		if (key == "ip")
		{
			ip = value;
		}
		else if (key == "port")
		{
			port = atoi(value.c_str());
		}
		else if (key == "username")
		{
			username = value;
		}
		else if (key == "password")
		{
			password = value;
		}
		else if (key == "dbname")
		{
			dbname = value;
		}
		else if (key == "initSize")
		{
			initSize = atoi(value.c_str());
		}
		else if (key == "maxSize")
		{
			maxSize = atoi(value.c_str());
		}
		else if (key == "maxIdleTime")
		{
			maxIdleTime = atoi(value.c_str());
		}
		else if (key == "connectionTimeOut")
		{
			connectionTimeout = atoi(value.c_str());
		}
	}
	return true;
}

//连接池构造函数
connectionpool::connectionpool()
{
	// 加载配置项了
	if (!loadConfigFile())
	{
		return;
	}

	// 创建初始数量的连接
	for (int i = 0; i < initSize; ++i)
	{
		connection* p = new connection();
		p->connect(ip, port, username, password, dbname);
		//p->refreshAliveTime(); // 刷新一下开始空闲的起始时间
		connectionQue.push(p);
		connectioncnt++;
	}

	//启动新进程，作为生产者进程
	thread produce(bind(&connectionpool::productConnectionTask, this));
	produce.detach();

	thread scanner(bind(&connectionpool::scannerConnectionTask, this));
	scanner.detach();
}

// 运行在独立的线程中，专门负责生产新连接
void connectionpool::productConnectionTask()
{
	for (;;)
	{
		unique_lock<mutex> lock(queueMutex);
		while (!connectionQue.empty())
		{
			cv.wait(lock);
		}

		if (connectioncnt < maxSize)
		{
			connection* p = new connection();
			p->connect(ip, port, username, password, dbname);
			p->refAlivetime(); // 刷新一下开始空闲的起始时间
			connectionQue.push(p);
			connectioncnt++;

		}
		
		//告知消费者可以连接
		cv.notify_all();
	}
}

//外部提供一个接口，方便外部获得空闲连接
shared_ptr<connection> connectionpool::getConnection()
{
	unique_lock<mutex> lock(queueMutex);
	while (connectionQue.empty())
	{
		if (cv_status::timeout == cv.wait_for(lock, chrono::milliseconds(connectionTimeout)))
		{
			if (connectionQue.empty())
			{
				LOG("获取空闲连接超时了...获取连接失败!");
				return nullptr;
			}

		}
	}
	/*shared_ptr智能指针析构时，会把connection资源直接delete掉，
	相当于调用connection的析构函数，connection就被close掉了。
	自定义shared_ptr的释放资源的方式，把connection直接归还到queue当中*/
		shared_ptr<connection> sp(connectionQue.front(),
			[&](connection* pcon) {
				// 这里是在服务器应用线程中调用的，所以一定要考虑队列的线程安全操作
				unique_lock<mutex> lock(queueMutex);
				pcon->refAlivetime();// 刷新一下开始空闲的起始时间
				connectionQue.push(pcon);
			});

	connectionQue.pop();
	cv.notify_all();  // 消费完连接以后，通知生产者线程检查一下，如果队列为空了，赶紧生产连接

	return sp;

}

//扫描maxIdleTime时间的空闲连接，进行对于的来连接回收
void connectionpool::scannerConnectionTask()
{
	for (;;)
	{
		//通过sleep模拟定时效果
		this_thread::sleep_for(chrono::seconds(maxIdleTime));

		//扫描整个对列，释放多于连接
		unique_lock<mutex> lock(queueMutex);
		while (connectioncnt > initSize)
		{
			connection* p = connectionQue.front();
			if (p->getAlivetime() >= (maxIdleTime * 100))
			{
				connectionQue.pop();
				connectioncnt--;
				delete p;
			}
			else
			{
				break;
			}
		}

	}
}