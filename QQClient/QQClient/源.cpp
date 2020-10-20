#include <iostream>
#include <windows.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
//#include <locale.h>
#include <winsock.h>
#pragma comment(lib,"Ws2_32.lib")
using namespace std;


//服务器IP
string ss;

//客户信息结构体
typedef struct CliInfo
{
	char m_id[1024];
	char m_pwd[1024];
	int falg;
	SOCKET m_sock;
}cliInfo;

//客户端信息
cliInfo cliinfo = { 0 };
//登录
bool Log()
{
	system("CLS");
	cout << "欢迎登陆" << endl;
	cliinfo.falg = 0;
	cout << "请输入ID：";
	cin >> cliinfo.m_id;
	cout << "请输入密码：";
	//cin >> cliinfo.m_pwd;
	char ch;
	int i = 0;
	memset(cliinfo.m_pwd, 0, 1024);
	while (1)
	{
		ch = _getch();
		if (ch == '\r')
		{
			break;
		}
		cliinfo.m_pwd[i] = ch;
		cout << "*";
		i++;
	}
	int ret = send(cliinfo.m_sock, (char*)&cliinfo, sizeof(cliinfo), 0);
	if (ret <= 0)
	{
		cerr << "发送登录信息失败" << endl;
		exit(-1);
	}
	char szBuf[1024] = { 0 };
	ret = recv(cliinfo.m_sock, szBuf, 1023, 0);
	if (ret <= 0)
	{
		cerr << "接受信息失败" << endl;
		exit(-1);
	}
	szBuf[ret] = '\0';
	if (strcmp(szBuf, "登录成功"))
	{
		cout << endl << "登录失败！！！,请重新启动登录" << endl;
		exit(-1);
		return false;
	}
	system("CLS");
	cout << endl << szBuf << endl;
	cout << "等待3秒跳转聊天室" << endl;
	for (int i = 3; i > 0; i--)
	{
		cout << "\b\b\b";
		cout << i << "秒";
		Sleep(1000);
	}
	system("CLS");
	system("title QQ聊天室");
	return true;
}
//沟通
DWORD CALLBACK SetL(LPVOID ls)
{
	//cliInfo cliinfo = *(cliInfo*)ls;
	char szBuf[1024] = { 0 };
	int ret;
	while (1)
	{
		memset(szBuf, 0, 1024);
		cin >> szBuf;
		ret = send(cliinfo.m_sock, szBuf, strlen(szBuf), 0);
		if (ret <= 0)
		{
			cerr << "发送聊天室消息失败" << endl;
			exit(-1);
		}
	}
	return 0;
}
DWORD CALLBACK GetL(LPVOID ls)
{
	//cliInfo cliinfo = *(cliInfo*)ls;
	char szBuf[1024] = { 0 };
	int ret;

	while (1)
	{
		memset(szBuf, 0, 1024);
		ret = recv(cliinfo.m_sock, szBuf, 1023, 0);
		if (ret <= 0)
		{
			cerr << "接收聊天室信息失败！！！" << endl;
			exit(-1);
		}
		szBuf[ret] = '\0';
		cout << szBuf << endl;
	}
	return 0;
}
bool Reg()
{
	system("CLS");
	cliinfo.falg = 1;
	int pos = 0;
	memset(cliinfo.m_id, 0, 1024);
	cout << "欢迎注册" << endl;
	cout << "请输入用户ID：";
	cin >> cliinfo.m_id;
post:
	cout << "请输入密码：";
	char a[256] = { 0 };
	char b[256] = { 0 };
	char ch;
	int i = 0;
	getchar();
	fflush(stdin);
	while (1)
	{
		ch = _getch();
		if (ch == '\r')
		{
			break;
		}
		a[i] = ch;
		cout << "*";
		i++;
	}
	pos++;
	cout << endl;
	cout << "请再次输入密码：";
	i = 0;
	fflush(stdin);
	while (1)
	{
		ch = _getch();
		if (ch == '\r')
		{
			break;
		}
		b[i] = ch;
		cout << "*";
		i++;
	}
	cout << endl;
	cout << "a=" << a << "  b=" << b << endl;
	getchar();
	if (strcmp(a, b) == 0)
	{
		memset(cliinfo.m_pwd, 0, 1024);
		i = 0;
		while (a[i] != '\0')
		{
			cliinfo.m_pwd[i] = a[i];
			i++;
		}
		int ret = send(cliinfo.m_sock, (char*)&cliinfo, sizeof(cliinfo), 0);
		if (ret <= 0)
		{
			cerr << "注册信息发送失败" << endl;
			closesocket(cliinfo.m_sock);
			exit(-1);
		}
		char szBuf[1024] = { 0 };
		ret = recv(cliinfo.m_sock, szBuf, 1023, 0);
		if (ret <= 0)
		{
			cerr << "注册信息接收失败" << endl;
			closesocket(cliinfo.m_sock);
			exit(-1);

		}
		if (strcmp(szBuf, "恭喜您注册成功") == 0)
		{

			cout << endl << "恭喜您注册成功" << endl;
			cout << "等待6秒返回主页" << endl;
			for (int i = 6; i > 0; i--)
			{
				cout << "\b\b\b";
				cout << i << "秒";
				Sleep(1000);
			}
			system("CLS");
			return true;
		}
		if (strcmp(szBuf, "用户名已存在！！！") == 0)
		{

			cout << endl << "用户名已存在！！！" << endl;
			cout << "等待6秒返回主页面" << endl;
			for (int i = 6; i > 0; i--)
			{
				cout << "\b\b\b";
				cout << i << "秒";
				Sleep(1000);
			}
			system("CLS");
			return true;
		}
		return false;
	}
	if (pos < 3)
	{
		cout << "两次输入的密码不一样" << endl;
		goto post;
	}
	cout << "三次机会用完了，请休息片刻" << endl;
	cout << "等待6秒返回主页面" << endl;
	for (int i = 6; i > 0; i--)
	{
		cout << "\b\b\b";
		cout << i << "秒";
		Sleep(1000);
	}
	system("CLS");
	return false;
}
void SerIp()
{
	cout << "请输入要连接服务器IP地址：";
	cin >> ss;
}

bool InitCli()
{

	//setlocale(LC_ALL, "");
	//设置标题
	system("title QQClient");
	//sock版本号
	WSAData wsData = { 0 };
	if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0)
	{
		cout << "载入socket库失败" << endl;
		system("pause");
		exit(-1);
	}

	//初始化套接字
	SOCKET cliSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (cliSock == INVALID_SOCKET || cliSock == SOCKET_ERROR)
	{
		cerr << "创建客户端套接字失败" << endl;
		exit(-1);
	}
	//绑定IP，端口
	SOCKADDR_IN serAddr = { 0 };
	serAddr.sin_family = AF_INET;
	serAddr.sin_addr.s_addr = inet_addr(ss.c_str());
	serAddr.sin_port = htons(12345);
	//请求连接
	if (connect(cliSock, (SOCKADDR*)&serAddr, sizeof(serAddr)))
	{
		cerr << "请求连接失败" << endl;
		exit(1);
	}

	cout << "连接服务器成功" << endl;

	cout << "等待3秒返回主页面" << endl;
	for (int i = 3; i > 0; i--)
	{
		cout << "\b\b\b";
		cout << i << "秒";
		Sleep(1000);
	}
	system("CLS");

	cliinfo.m_sock = cliSock;
	return true;
}
int main()
{
	
	SerIp();
	InitCli();
	char pos;
	while (1)
	{
		
		cout << "-------------------------------------" << endl;
		cout << "         欢迎使用QQ聊天室" << endl;
		cout << "       1 登录" << endl;
		cout << "       2 注册" << endl;
		cout << "       q 退出" << endl;
		cout << "---------------------------------------" << endl;
		cin >> pos;
		fflush(stdin);
		switch (pos)
		{
		case '1':
			//登录

			if (Log())
			{
				HANDLE Threads[2];
				Threads[0] = CreateThread(NULL, 0, SetL,&cliinfo, 0, NULL);
				Threads[1] = CreateThread(NULL, 0, GetL, &cliinfo, 0, NULL);
				WaitForMultipleObjects(2, Threads, TRUE, INFINITE);
				CloseHandle(Threads[0]);
				CloseHandle(Threads[1]);
			}
			
			break;
		case '2':
			Reg();
			closesocket(cliinfo.m_sock);
			InitCli();
			break;
		case 'Q':
		case 'q':
			exit(0);
			break;
		default:
			cout << "输入有误，请重新输入" << endl;
		}
	}
	return 0;
}