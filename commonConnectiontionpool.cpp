#include"pch.h"
#include"commonConnectiontionpool.h"
#include<iostream>
using namespace std;

//�̰߳�ȫ����������ģʽ
connectionpool* connectionpool::getConnectionpool()
{
	static connectionpool pool;
	return &pool;
}

//�������ļ��м���������
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
		if (idx == -1) // ��Ч��������
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

//���ӳع��캯��
connectionpool::connectionpool()
{
	// ������������
	if (!loadConfigFile())
	{
		return;
	}

	// ������ʼ����������
	for (int i = 0; i < initSize; ++i)
	{
		connection* p = new connection();
		p->connect(ip, port, username, password, dbname);
		//p->refreshAliveTime(); // ˢ��һ�¿�ʼ���е���ʼʱ��
		connectionQue.push(p);
		connectioncnt++;
	}

	//�����½��̣���Ϊ�����߽���
	thread produce(bind(&connectionpool::productConnectionTask, this));
	produce.detach();

	thread scanner(bind(&connectionpool::scannerConnectionTask, this));
	scanner.detach();
}

// �����ڶ������߳��У�ר�Ÿ�������������
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
			p->refAlivetime(); // ˢ��һ�¿�ʼ���е���ʼʱ��
			connectionQue.push(p);
			connectioncnt++;

		}
		
		//��֪�����߿�������
		cv.notify_all();
	}
}

//�ⲿ�ṩһ���ӿڣ������ⲿ��ÿ�������
shared_ptr<connection> connectionpool::getConnection()
{
	unique_lock<mutex> lock(queueMutex);
	while (connectionQue.empty())
	{
		if (cv_status::timeout == cv.wait_for(lock, chrono::milliseconds(connectionTimeout)))
		{
			if (connectionQue.empty())
			{
				LOG("��ȡ�������ӳ�ʱ��...��ȡ����ʧ��!");
				return nullptr;
			}

		}
	}
	/*shared_ptr����ָ������ʱ�����connection��Դֱ��delete����
	�൱�ڵ���connection������������connection�ͱ�close���ˡ�
	�Զ���shared_ptr���ͷ���Դ�ķ�ʽ����connectionֱ�ӹ黹��queue����*/
		shared_ptr<connection> sp(connectionQue.front(),
			[&](connection* pcon) {
				// �������ڷ�����Ӧ���߳��е��õģ�����һ��Ҫ���Ƕ��е��̰߳�ȫ����
				unique_lock<mutex> lock(queueMutex);
				pcon->refAlivetime();// ˢ��һ�¿�ʼ���е���ʼʱ��
				connectionQue.push(pcon);
			});

	connectionQue.pop();
	cv.notify_all();  // �����������Ժ�֪ͨ�������̼߳��һ�£��������Ϊ���ˣ��Ͻ���������

	return sp;

}

//ɨ��maxIdleTimeʱ��Ŀ������ӣ����ж��ڵ������ӻ���
void connectionpool::scannerConnectionTask()
{
	for (;;)
	{
		//ͨ��sleepģ�ⶨʱЧ��
		this_thread::sleep_for(chrono::seconds(maxIdleTime));

		//ɨ���������У��ͷŶ�������
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