#pragma once
/*
实现MySQL数据库的操作
*/
#include<string>
#include<mysql.h>
#include<ctime>
using namespace std;

class connection
{
public:
	//初始化数据库
	connection();
	//释放数据库连接资源
	~connection();
	//连接数据库
	bool connect(
		string ip,
		unsigned short port, 
		string user,
		string password,
		string dbname);

	//更新操作 insert delete update
	bool update(string sql);
	//查询操作
	MYSQL_RES* query(string sql);

	void refAlivetime();
	clock_t getAlivetime() const;
	
private:
	MYSQL* _conn;//表示和mysql server的一条连接
	clock_t alivetlime;
};