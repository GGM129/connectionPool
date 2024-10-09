#include"public.h"
#include"pch.h"
#include"connection.h"
#include<iostream>
using namespace std;
//��ʼ�����ݿ�
connection::connection() 
{
	// ��ʼ�����ݿ�����
	_conn = mysql_init(nullptr);
}
//�ͷ����ݿ�������Դ
connection::~connection()
{
	if (_conn != nullptr)
		mysql_close(_conn);
}
//�������ݿ�
bool connection:: connect(string ip, unsigned short port, 
	string username,string password, string dbname)
{
	// �������ݿ�
	MYSQL* p = mysql_real_connect(_conn, ip.c_str(), username.c_str(),
		password.c_str(), dbname.c_str(), port, NULL, 0);
	return p != nullptr;
}

//���²��� insert delete update
bool connection:: update(string sql)
{
	// ���²��� insert��delete��update
	if (mysql_query(_conn, sql.c_str()))
	{
		cout << "error" << mysql_error(_conn) << endl;
		LOG("����ʧ��:" + sql);
		return false;
	}
	return true;
}
//��ѯ����
MYSQL_RES*  connection::query(string sql)
{
	if (mysql_query(_conn, sql.c_str()))
	{
		LOG("��ѯʧ��:" + sql);
		return nullptr;
	}
	return mysql_use_result(_conn);
}

void connection::refAlivetime()
{
	alivetlime = clock();
}
clock_t connection::getAlivetime()const
{
	return clock() - alivetlime;
}