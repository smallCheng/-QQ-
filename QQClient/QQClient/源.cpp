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


//������IP
string ss;

//�ͻ���Ϣ�ṹ��
typedef struct CliInfo
{
	char m_id[1024];
	char m_pwd[1024];
	int falg;
	SOCKET m_sock;
}cliInfo;

//�ͻ�����Ϣ
cliInfo cliinfo = { 0 };
//��¼
bool Log()
{
	system("CLS");
	cout << "��ӭ��½" << endl;
	cliinfo.falg = 0;
	cout << "������ID��";
	cin >> cliinfo.m_id;
	cout << "���������룺";
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
		cerr << "���͵�¼��Ϣʧ��" << endl;
		exit(-1);
	}
	char szBuf[1024] = { 0 };
	ret = recv(cliinfo.m_sock, szBuf, 1023, 0);
	if (ret <= 0)
	{
		cerr << "������Ϣʧ��" << endl;
		exit(-1);
	}
	szBuf[ret] = '\0';
	if (strcmp(szBuf, "��¼�ɹ�"))
	{
		cout << endl << "��¼ʧ�ܣ�����,������������¼" << endl;
		exit(-1);
		return false;
	}
	system("CLS");
	cout << endl << szBuf << endl;
	cout << "�ȴ�3����ת������" << endl;
	for (int i = 3; i > 0; i--)
	{
		cout << "\b\b\b";
		cout << i << "��";
		Sleep(1000);
	}
	system("CLS");
	system("title QQ������");
	return true;
}
//��ͨ
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
			cerr << "������������Ϣʧ��" << endl;
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
			cerr << "������������Ϣʧ�ܣ�����" << endl;
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
	cout << "��ӭע��" << endl;
	cout << "�������û�ID��";
	cin >> cliinfo.m_id;
post:
	cout << "���������룺";
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
	cout << "���ٴ��������룺";
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
			cerr << "ע����Ϣ����ʧ��" << endl;
			closesocket(cliinfo.m_sock);
			exit(-1);
		}
		char szBuf[1024] = { 0 };
		ret = recv(cliinfo.m_sock, szBuf, 1023, 0);
		if (ret <= 0)
		{
			cerr << "ע����Ϣ����ʧ��" << endl;
			closesocket(cliinfo.m_sock);
			exit(-1);

		}
		if (strcmp(szBuf, "��ϲ��ע��ɹ�") == 0)
		{

			cout << endl << "��ϲ��ע��ɹ�" << endl;
			cout << "�ȴ�6�뷵����ҳ" << endl;
			for (int i = 6; i > 0; i--)
			{
				cout << "\b\b\b";
				cout << i << "��";
				Sleep(1000);
			}
			system("CLS");
			return true;
		}
		if (strcmp(szBuf, "�û����Ѵ��ڣ�����") == 0)
		{

			cout << endl << "�û����Ѵ��ڣ�����" << endl;
			cout << "�ȴ�6�뷵����ҳ��" << endl;
			for (int i = 6; i > 0; i--)
			{
				cout << "\b\b\b";
				cout << i << "��";
				Sleep(1000);
			}
			system("CLS");
			return true;
		}
		return false;
	}
	if (pos < 3)
	{
		cout << "������������벻һ��" << endl;
		goto post;
	}
	cout << "���λ��������ˣ�����ϢƬ��" << endl;
	cout << "�ȴ�6�뷵����ҳ��" << endl;
	for (int i = 6; i > 0; i--)
	{
		cout << "\b\b\b";
		cout << i << "��";
		Sleep(1000);
	}
	system("CLS");
	return false;
}
void SerIp()
{
	cout << "������Ҫ���ӷ�����IP��ַ��";
	cin >> ss;
}

bool InitCli()
{

	//setlocale(LC_ALL, "");
	//���ñ���
	system("title QQClient");
	//sock�汾��
	WSAData wsData = { 0 };
	if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0)
	{
		cout << "����socket��ʧ��" << endl;
		system("pause");
		exit(-1);
	}

	//��ʼ���׽���
	SOCKET cliSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (cliSock == INVALID_SOCKET || cliSock == SOCKET_ERROR)
	{
		cerr << "�����ͻ����׽���ʧ��" << endl;
		exit(-1);
	}
	//��IP���˿�
	SOCKADDR_IN serAddr = { 0 };
	serAddr.sin_family = AF_INET;
	serAddr.sin_addr.s_addr = inet_addr(ss.c_str());
	serAddr.sin_port = htons(12345);
	//��������
	if (connect(cliSock, (SOCKADDR*)&serAddr, sizeof(serAddr)))
	{
		cerr << "��������ʧ��" << endl;
		exit(1);
	}

	cout << "���ӷ������ɹ�" << endl;

	cout << "�ȴ�3�뷵����ҳ��" << endl;
	for (int i = 3; i > 0; i--)
	{
		cout << "\b\b\b";
		cout << i << "��";
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
		cout << "         ��ӭʹ��QQ������" << endl;
		cout << "       1 ��¼" << endl;
		cout << "       2 ע��" << endl;
		cout << "       q �˳�" << endl;
		cout << "---------------------------------------" << endl;
		cin >> pos;
		fflush(stdin);
		switch (pos)
		{
		case '1':
			//��¼

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
			cout << "������������������" << endl;
		}
	}
	return 0;
}