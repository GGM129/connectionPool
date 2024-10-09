#include<iostream>
using namespace std;
#include"connection.h"
#include"pch.h"
#include "commonConnectiontionpool.h"

int main()
{
	/*connection conn;
	char sql[1024] = { 0 };
	sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
		"lisi", 20, "female");
	conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
	conn.update(sql);

	return 0;*/ 

	clock_t begin = clock();
	connectionpool* cp = connectionpool::getConnectionpool();
	thread t1([]() {
		//connectionpool* cp = connectionpool::getConnectionpool();
		for (int i = 0; i < 2500; ++i)
		{
			/*char sql[1024] = {0};
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
			shared_ptr<connection> sp = cp->getConnection();
			sp->update(sql);*/
			connection conn;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"lisi", 20, "female");
			conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
			conn.update(sql);
		}
		});
	thread t2([]() {
		//connectionpool* cp = connectionpool::getConnectionpool();
		for (int i = 0; i < 2500; ++i)
		{
			/*char sql[1024] = {0};
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
			shared_ptr<connection> sp = cp->getConnection();
			sp->update(sql);*/
			connection conn;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"lisi", 20, "female");
			conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
			conn.update(sql);
		}
		});
	thread t3([]() {
		//connectionpool* cp = connectionpool::getConnectionpool();
		for (int i = 0; i < 2500; ++i)
		{
			/*char sql[1024] = {0};
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
			shared_ptr<connection> sp = cp->getConnection();
			sp->update(sql);*/
			connection conn;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"lisi", 20, "female");
			conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
			conn.update(sql);
		}
		});
	thread t4([]() {
		//connectionpool* cp = connectionpool::getConnectionpool();
		for (int i = 0; i < 2500; ++i)
		{
			/*char sql[1024] = {0};
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"zhang san", 20, "male");
			shared_ptr<connection> sp = cp->getConnection();
			sp->update(sql);*/
			connection conn;
			char sql[1024] = { 0 };
			sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
				"lisi", 20, "female");
			conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
			conn.update(sql);
		}
		});

	t1.join();
	t2.join();
	t3.join();
	t4.join();

	clock_t end = clock();
	cout << end - begin <<"ms" << endl;
	/*for (int i = 0; i < 5000; i++)
	{
		connection conn;
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
			"lisi", 20, "female");
		conn.connect("127.0.0.1", 3306, "root", "123456", "chat");
		conn.update(sql);
		/*shared_ptr<connection> sp = cp->getConnection();
		char sql[1024] = { 0 };
		sprintf(sql, "insert into user(name,age,sex) values('%s',%d,'%s')",
			"lisi", 20, "female");
		sp->update(sql);
	}*/
	return 0;

}
