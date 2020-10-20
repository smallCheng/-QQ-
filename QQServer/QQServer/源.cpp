//#include "Server.hpp"
#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <list>
#include <string>
#include <string.h>
#include <fstream>
//#include <locale.h>
#include<WinSock.h>
#pragma  comment(lib,"Ws2_32.lib")
using namespace std;

//服务器流套接字
SOCKET serSock = 0;
//接受客户端属性结构体
SOCKADDR_IN cliAddr;

//客户信息结构体
typedef struct CliInfo
{
	char m_id[1024];
	char m_pwd[1024];
	int falg;
	SOCKET m_sock;
}cliInfo;

//接受客户登录信息
//cliInfo cliinfo;


// 链表容器(客户端信息)
list<cliInfo*> listInfo;

//打印在线人数
void PrintNum()
{
	cout << "在线人数:" << listInfo.size() << "人" << endl;
}
//打印在线列表
void PrintPeo()
{
	if (listInfo.size() == 0)
	{
		cout << "当前无人在线" << endl;
		return;
	}
	list<cliInfo*>::iterator it = listInfo.begin();
	for (it; it != listInfo.end(); it++)
	{
		cout << (*it)->m_id << endl;
	}
}

//功能线程
DWORD CALLBACK Functions(LPVOID ls)
{
	char pos;
	while (1)
	{
		cout << "------------------------------------------------------" << endl;
		cout << "                 欢迎使用QQ聊天室服务器                     " << endl;
		cout << "------------------------------------------------------" << endl;
		cout << "                    1 打印在线人数                      " << endl;
		cout << "                    2 打印在在线列表                    " << endl;
		cout << "                    q 退出服务器                         " << endl;
		cout << "----------------------------------------------------------" << endl;
		cout << endl << "请输入：";
		cin >> pos;
		switch (pos)
		{
		case '1':
		{
			PrintNum();
		}
		break;
		case '2':
		{
			PrintPeo();
		}
		break;
		case 'q':
		case 'Q':
		{
			closesocket(serSock);
			exit(0);
		}

		break;
		default:
			cout << endl << "输入错误，请重新输入：" << endl;
		}
	}

	return 0;
}
//聊天室API
bool cliThread(cliInfo* ls)
{
	cliInfo* cliinfo = (cliInfo*)ls;
	SOCKET cliSock = cliinfo->m_sock;
	string szStr = cliinfo->m_id;
	string szStr1;
	szStr1 = "欢迎" + szStr + "进入聊天室";
	list<cliInfo*>::iterator it = listInfo.begin();
	for (it; it != listInfo.end(); it++)
	{
		send((*it)->m_sock, szStr1.c_str(), szStr1.size(), 0);
	}
	char szBuf[1024] = { 0 };

	while (1)
	{
		memset(szBuf, 0, 1024);
		int ret = recv(cliSock, szBuf, sizeof(szBuf), 0);
		if (ret <= 0)
		{
			szStr1 = "";
			szStr1 = szStr + "离开聊天室";
			list<cliInfo*>::iterator it = listInfo.begin();
			for (it; it != listInfo.end(); it++)
			{

				if (strcmp((*it)->m_id, cliinfo->m_id))
				{
					send((*it)->m_sock, szStr1.c_str(), szStr1.size(), 0);
				}
			}
			for (it; it != listInfo.end(); it++)
			{
				if (strcmp((*it)->m_id, cliinfo->m_id) == 0)
				{
					listInfo.erase(it);
					break;
				}
			}
			closesocket(cliSock);
			goto end1;
		}
		szStr1 = "";
		szStr1 = szStr + ":\n\t" + szBuf;
		list<cliInfo*>::iterator it = listInfo.begin();
		for (it; it != listInfo.end(); it++)
		{
			if (strcmp((*it)->m_id, cliinfo->m_id))
			{
				send((*it)->m_sock, szStr1.c_str(), szStr1.size(), 0);
			}
		}
	}
end1:
	return false;
}
//初始化
bool InitInfo()
{
	//setlocale(LC_ALL, "");
	//设置标题
	system("title QQServer");

	//客户端资料
	ofstream outFile("cliinfo.txt", ios::app);
	if (!outFile)
	{
		cerr << "创建cliinfo.txt失败" << endl;
		return false;
	}
	outFile.close();


	//sock版本号
	WSAData wsData = { 0 };
	if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0)
	{
		cout << "载入socket库失败" << endl;
		system("pause");
		return false;
	}

	//初始化套接字

	serSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serSock == INVALID_SOCKET || serSock == SOCKET_ERROR)
	{
		cerr << "创建服务器套接字失败" << endl;
		return false;
	}

	//绑定IP，端口
	SOCKADDR_IN serAddr = { 0 };
	serAddr.sin_family = AF_INET;
	serAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serAddr.sin_port = htons(12345);
	if (bind(serSock, (SOCKADDR*)&serAddr, sizeof(serAddr)))
	{
		cerr << "绑定IP，端口失败" << endl;
		return false;
	}
	//监听
	if (listen(serSock, 66))
	{
		cerr << "监听失败" << endl;
		closesocket(serSock);
		return false;
	}
	return  true;
}
//登录线程函数
DWORD CALLBACK LogThread(LPVOID ls)
{
	cliInfo* cliinfo = (cliInfo*)ls;
	bool pos = false;
	ifstream inFile("cliinfo.txt", ios::in);
	if (!inFile)
	{
		cerr << "读cliinfo.txt失败" << endl;
		//closesocket(serSock);
		closesocket(cliinfo->m_sock);
		delete cliinfo;
		cliinfo = NULL;
		ExitThread(-1);
	}
	char id[1024] = { 0 };
	char pwd[1024] = { 0 };
	while (!inFile.eof())
	{
		memset(id, 0, 1024);
		memset(pwd, 0, 1024);
		inFile.getline(id, 1023);
		inFile.getline(pwd, 1023);
		if (strcmp(cliinfo->m_id, id) == 0 && strcmp(cliinfo->m_pwd, pwd) == 0)
		{
			inFile.close();
			pos = true;
			string szBuf = "登录成功";
			send(cliinfo->m_sock, szBuf.c_str(), szBuf.size(), 0);
			//客户信息插入到链表容器
			listInfo.push_back(cliinfo);
			//聊天室API
			bool clithread = cliThread(cliinfo);
			if (!clithread)
			{
				//cerr << "创建客户端线程失败" << endl;
				//closesocket(serSock);
				closesocket(cliinfo->m_sock);
				delete cliinfo;
				cliinfo = NULL;
				ExitThread(-1);
			}
		}
	}
	inFile.close();

	if (!pos)
	{
		string szBuf = "用户名或密码错误！";
		send(cliinfo->m_sock, szBuf.c_str(), szBuf.size(), 0);

	}
	//closesocket(serSock);
	closesocket(cliinfo->m_sock);
	delete cliinfo;
	cliinfo = NULL;
	ExitThread(0);

}
//注册线程处理函数
DWORD CALLBACK RegThread(LPVOID ls)
{
	cliInfo* cliinfo = (cliInfo*)ls;
	ifstream inFile("cliinfo.txt", ios::in);
	if (!inFile)
	{
		cerr << "打开cliinfo.txt失败" << endl;
		//closesocket(serSock);
		closesocket(cliinfo->m_sock);
		delete cliinfo;
		cliinfo = NULL;
		ExitThread(-1);
	}
	bool pos = true;
	char id[1024] = { 0 };
	char pwd[1024] = { 0 };
	while (!inFile.eof())
	{
		memset(id, 0, 1024);
		memset(pwd, 0, 1024);

		inFile.getline(id, 1023);
		inFile.getline(pwd, 1023);
		if (strcmp(id, cliinfo->m_id) == 0)
		{
			//cout << endl << id << "  " << cliinfo->m_id << endl;
			pos = false;
			char str[1024] = "用户名已存在！！！";
			send(cliinfo->m_sock, str, strlen(str), 0);
			inFile.close();
			//closesocket(serSock);
			closesocket(cliinfo->m_sock);
			delete cliinfo;
			cliinfo = NULL;
			ExitThread(-1);
		}
	}
	if (pos == true)
	{
		ofstream outFile("cliinfo.txt", ios::app);
		if (!outFile)
		{
			cerr << "打开cliinfo.txt失败" << endl;
			//closesocket(serSock);
			closesocket(cliinfo->m_sock);
			ExitThread(-1);
		}
		outFile << (char*)cliinfo->m_id << endl << (char*)cliinfo->m_pwd << endl;
		outFile.close();
		inFile.close();
		string szBuf = "恭喜您注册成功";
		send(cliinfo->m_sock, szBuf.c_str(), szBuf.size(), 0);
	}
	ExitThread(0);
	return 0;
}
//接受请求线程处理函数
void accFun()
{

	while (1)
	{
	top1:
		cliInfo* cliinfo = new cliInfo;
		memset(&cliAddr, 0, sizeof(cliAddr));
		memset(cliinfo, 0, sizeof(cliInfo));
		SOCKET cliSock = accept(serSock, (SOCKADDR*)&cliAddr, NULL);
		if (!cliSock)
		{
			cerr << "接受请求失败" << endl;
			//closesocket(serSock);
			goto top1;
		}
		int ret = recv(cliSock, (char*)cliinfo, sizeof(cliInfo), 0);
		if (ret <= 0)
		{
			//cerr << "接受客户端登录信息失败" << endl;
			//closesocket(serSock);
			closesocket(cliSock);
			goto top1;
		}
		cliinfo->m_sock = cliSock;
		switch (cliinfo->falg)
		{
		case 0:
			//登录聊天室线程
		{
			HANDLE logThread = CreateThread(NULL, 0, LogThread, cliinfo, 0, NULL);
		}
		break;
		case 1:
			//注册线程函数
		{
			HANDLE regThread = CreateThread(NULL, 0, RegThread, cliinfo, 0, NULL);
		}
		break;
		default:
		{
			closesocket(cliSock);
			goto top1;

		}
		break;
		}

	}

	closesocket(serSock);
}
int main()
{
	//初始化服务器
	if(InitInfo()==false)
	{
		cerr << "初始化失败" << endl;
		exit(-1);
	}

	//功能线程函数
	HANDLE Thread1 = CreateThread(NULL, 0, Functions, NULL, 0, NULL);
	if (!Thread1)
	{
		cerr << "功能线程函数创建失败" << endl;
		exit(-1);
	}

	//接受请求处理API
	accFun();
	WaitForSingleObject(Thread1,INFINITE);
	CloseHandle(Thread1);
	return 0;
}