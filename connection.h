#pragma once
/*
ʵ��MySQL���ݿ�Ĳ���
*/
#include<string>
#include<mysql.h>
#include<ctime>
using namespace std;

class connection
{
public:
	//��ʼ�����ݿ�
	connection();
	//�ͷ����ݿ�������Դ
	~connection();
	//�������ݿ�
	bool connect(
		string ip,
		unsigned short port, 
		string user,
		string password,
		string dbname);

	//���²��� insert delete update
	bool update(string sql);
	//��ѯ����
	MYSQL_RES* query(string sql);

	void refAlivetime();
	clock_t getAlivetime() const;
	
private:
	MYSQL* _conn;//��ʾ��mysql server��һ������
	clock_t alivetlime;
};