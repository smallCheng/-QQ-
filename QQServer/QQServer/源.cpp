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

//���������׽���
SOCKET serSock = 0;
//���ܿͻ������Խṹ��
SOCKADDR_IN cliAddr;

//�ͻ���Ϣ�ṹ��
typedef struct CliInfo
{
	char m_id[1024];
	char m_pwd[1024];
	int falg;
	SOCKET m_sock;
}cliInfo;

//���ܿͻ���¼��Ϣ
//cliInfo cliinfo;


// ��������(�ͻ�����Ϣ)
list<cliInfo*> listInfo;

//��ӡ��������
void PrintNum()
{
	cout << "��������:" << listInfo.size() << "��" << endl;
}
//��ӡ�����б�
void PrintPeo()
{
	if (listInfo.size() == 0)
	{
		cout << "��ǰ��������" << endl;
		return;
	}
	list<cliInfo*>::iterator it = listInfo.begin();
	for (it; it != listInfo.end(); it++)
	{
		cout << (*it)->m_id << endl;
	}
}

//�����߳�
DWORD CALLBACK Functions(LPVOID ls)
{
	char pos;
	while (1)
	{
		cout << "------------------------------------------------------" << endl;
		cout << "                 ��ӭʹ��QQ�����ҷ�����                     " << endl;
		cout << "------------------------------------------------------" << endl;
		cout << "                    1 ��ӡ��������                      " << endl;
		cout << "                    2 ��ӡ�������б�                    " << endl;
		cout << "                    q �˳�������                         " << endl;
		cout << "----------------------------------------------------------" << endl;
		cout << endl << "�����룺";
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
			cout << endl << "����������������룺" << endl;
		}
	}

	return 0;
}
//������API
bool cliThread(cliInfo* ls)
{
	cliInfo* cliinfo = (cliInfo*)ls;
	SOCKET cliSock = cliinfo->m_sock;
	string szStr = cliinfo->m_id;
	string szStr1;
	szStr1 = "��ӭ" + szStr + "����������";
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
			szStr1 = szStr + "�뿪������";
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
//��ʼ��
bool InitInfo()
{
	//setlocale(LC_ALL, "");
	//���ñ���
	system("title QQServer");

	//�ͻ�������
	ofstream outFile("cliinfo.txt", ios::app);
	if (!outFile)
	{
		cerr << "����cliinfo.txtʧ��" << endl;
		return false;
	}
	outFile.close();


	//sock�汾��
	WSAData wsData = { 0 };
	if (WSAStartup(MAKEWORD(2, 2), &wsData) != 0)
	{
		cout << "����socket��ʧ��" << endl;
		system("pause");
		return false;
	}

	//��ʼ���׽���

	serSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serSock == INVALID_SOCKET || serSock == SOCKET_ERROR)
	{
		cerr << "�����������׽���ʧ��" << endl;
		return false;
	}

	//��IP���˿�
	SOCKADDR_IN serAddr = { 0 };
	serAddr.sin_family = AF_INET;
	serAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serAddr.sin_port = htons(12345);
	if (bind(serSock, (SOCKADDR*)&serAddr, sizeof(serAddr)))
	{
		cerr << "��IP���˿�ʧ��" << endl;
		return false;
	}
	//����
	if (listen(serSock, 66))
	{
		cerr << "����ʧ��" << endl;
		closesocket(serSock);
		return false;
	}
	return  true;
}
//��¼�̺߳���
DWORD CALLBACK LogThread(LPVOID ls)
{
	cliInfo* cliinfo = (cliInfo*)ls;
	bool pos = false;
	ifstream inFile("cliinfo.txt", ios::in);
	if (!inFile)
	{
		cerr << "��cliinfo.txtʧ��" << endl;
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
			string szBuf = "��¼�ɹ�";
			send(cliinfo->m_sock, szBuf.c_str(), szBuf.size(), 0);
			//�ͻ���Ϣ���뵽��������
			listInfo.push_back(cliinfo);
			//������API
			bool clithread = cliThread(cliinfo);
			if (!clithread)
			{
				//cerr << "�����ͻ����߳�ʧ��" << endl;
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
		string szBuf = "�û������������";
		send(cliinfo->m_sock, szBuf.c_str(), szBuf.size(), 0);

	}
	//closesocket(serSock);
	closesocket(cliinfo->m_sock);
	delete cliinfo;
	cliinfo = NULL;
	ExitThread(0);

}
//ע���̴߳�����
DWORD CALLBACK RegThread(LPVOID ls)
{
	cliInfo* cliinfo = (cliInfo*)ls;
	ifstream inFile("cliinfo.txt", ios::in);
	if (!inFile)
	{
		cerr << "��cliinfo.txtʧ��" << endl;
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
			char str[1024] = "�û����Ѵ��ڣ�����";
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
			cerr << "��cliinfo.txtʧ��" << endl;
			//closesocket(serSock);
			closesocket(cliinfo->m_sock);
			ExitThread(-1);
		}
		outFile << (char*)cliinfo->m_id << endl << (char*)cliinfo->m_pwd << endl;
		outFile.close();
		inFile.close();
		string szBuf = "��ϲ��ע��ɹ�";
		send(cliinfo->m_sock, szBuf.c_str(), szBuf.size(), 0);
	}
	ExitThread(0);
	return 0;
}
//���������̴߳�����
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
			cerr << "��������ʧ��" << endl;
			//closesocket(serSock);
			goto top1;
		}
		int ret = recv(cliSock, (char*)cliinfo, sizeof(cliInfo), 0);
		if (ret <= 0)
		{
			//cerr << "���ܿͻ��˵�¼��Ϣʧ��" << endl;
			//closesocket(serSock);
			closesocket(cliSock);
			goto top1;
		}
		cliinfo->m_sock = cliSock;
		switch (cliinfo->falg)
		{
		case 0:
			//��¼�������߳�
		{
			HANDLE logThread = CreateThread(NULL, 0, LogThread, cliinfo, 0, NULL);
		}
		break;
		case 1:
			//ע���̺߳���
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
	//��ʼ��������
	if(InitInfo()==false)
	{
		cerr << "��ʼ��ʧ��" << endl;
		exit(-1);
	}

	//�����̺߳���
	HANDLE Thread1 = CreateThread(NULL, 0, Functions, NULL, 0, NULL);
	if (!Thread1)
	{
		cerr << "�����̺߳�������ʧ��" << endl;
		exit(-1);
	}

	//����������API
	accFun();
	WaitForSingleObject(Thread1,INFINITE);
	CloseHandle(Thread1);
	return 0;
}