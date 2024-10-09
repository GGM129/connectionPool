#pragma once
/*
ʵ�����ӳع���ģ��
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
	//�ⲿ�ṩһ���ӿڣ������ⲿ��ÿ�������
	shared_ptr<connection>getConnection();
private:
	connectionpool();


	bool loadConfigFile();

	//�����ڶ������߳��У�ר�Ÿ�������������
	void productConnectionTask();
	//ɨ��maxIdleTimeʱ��Ŀ������ӣ����ж��ڵ������ӻ���
	void scannerConnectionTask();

	string ip;
	unsigned short port;
	string username;
	string password;
	string dbname;
	int initSize;//���ӳس�ʼ������
	int maxSize;//���ӳ����������
	int maxIdleTime;//���ӳ�������ʱ��
	int connectionTimeout;//���ӳػ�ȡ���ӳ�ʱʱ��

	queue<connection*> connectionQue;
	mutex queueMutex;//ά�����е��̰߳�ȫ������
	atomic_int connectioncnt;//��¼��������
	condition_variable cv;//�����������������ڽ���ͨ��
};