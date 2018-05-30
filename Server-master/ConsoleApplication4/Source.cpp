// ConsoleApplication12.cpp: определяет точку входа для консольного приложения.
//
#pragma comment(lib,"Ws2_32.lib")
#include <sys/types.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <ctime>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include <windows.h>
#define STRICT 1 
using namespace std;

class Client
{
private:
	int key;
	SOCKET Client_Sock;
public:
	Client(SOCKET * in, int C) :Client_Sock(*in) { key = C; }
	Client() {}
	SOCKET getsock() { return Client_Sock; }
	int getkey() { return key; }
};

struct SENDBUFFER
{
	SENDBUFFER()
	{
		typemessage = 0;
		key = 0;
		ZeroMemory(name, sizeof(char) * 14);
		ZeroMemory(buffer, sizeof(char) * 202);
	}
	int key;
	int typemessage;
	char name[14];
	char buffer[202];
};

//Сокет для подключения, для хранения подключенных и для "прослушка" + подсчет количества подключенных 

SOCKET Connect;
SOCKET Listen;
int Count = 0;

std::vector<Client> Connection(64);

//Функция для отправки-приема сообщений... Принимает сообщение - рассылает всем подключенным

void decompres(int n)
{
	for (int i = n; i < Count - 1; i++)
		Connection[i] = Connection[i + 1];
	Connection[Count].~Client();
}

void SendM(int ID)
{
	for (;; Sleep(75))
	{
		SENDBUFFER s;
		int iResult = recv(Connection[ID].getsock(), (char*)&s, sizeof(s), NULL);
		std::cout << s.name << ' ' << s.typemessage << ' ' << Connection[ID].getkey() << std::endl;
		if (s.typemessage == 3)
		{
			for (int i = 0; i <= Count; i++)
				send(Connection[i].getsock(), (char*)&s, sizeof(s), NULL);
			closesocket(Connection[ID].getsock());
			decompres(ID);
			Count--;
			break;
		}
		if (iResult>0)
		{
			for (int i = 0; i <= Count; i++)
				send(Connection[i].getsock(), (char*)&s, sizeof(s), NULL);
		}
	}
	std::cout << ID << " off\n";
}
std::string encode(std::string s)
{
	std::string out;
	for (unsigned int i = 0; i < s.length(); i++)
		out += (char)(s[i] + 120) % 125 + 12;
	return out;
}


int main()
{
	auto begin = std::chrono::steady_clock::now(); //начало таймера
	if (IsDebuggerPresent())ExitProcess(0);
	BOOL f = false;
	CheckRemoteDebuggerPresent(GetCurrentProcess(), &f);
	if (f == true) ExitProcess(0);
	setlocale(LC_ALL, "russian");
	ifstream in("countersign.txt");
	string str;
	std::getline(in, str);
	in.close(); 
	std::cout << "Enter password\n";

	//скрывает вводимые символы
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
	DWORD mode = 0;
	GetConsoleMode(hStdin, &mode);
	SetConsoleMode(hStdin, mode & (~ENABLE_ECHO_INPUT));

	//ввод пароля
	string s;
	getline(cin, s);

	//таймер заканчивается
	auto end = std::chrono::steady_clock::now();
	auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
	std::cout << "The time: " << elapsed_ms.count()/1000 << " s\n";
	if (elapsed_ms.count()/1000 >= 10)
	{
		std::cout << "Время истекло";
		return 0;
	}

	if (encode(s) != str)
	{
		std::cout << "Error\n";
		return 0;
	}
	SetConsoleMode(hStdin, mode);

	std::cout << "Creating server:\n";
	WSAData  ws;
	WORD version = MAKEWORD(2, 2);
	int MasterSocket = WSAStartup(version, &ws);
	if (MasterSocket != 0)
	{
		return 0;
	}
	struct addrinfo hints;
	struct addrinfo * result;
	ZeroMemory(&hints, sizeof(hints));

	//Задание сокетов

	hints.ai_family = AF_INET;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	//установка ip и порта
	std::cout << "Введите ip:\n";
	std::string iport; std::cin >> iport;
	std::cout << "Введите port:\n";
	std::string port; std::cin >> port;
	getaddrinfo(iport.c_str(), port.c_str(), &hints, &result);

	//Заполнение сокета listen

	Listen = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	bind(Listen, result->ai_addr, result->ai_addrlen);
	listen(Listen, SOMAXCONN);
	freeaddrinfo(result);

	//Начало работы сервера

	std::cout << "Start" << std::endl;
	char c_connect[] = "Connect";
	while (1)
	{
		//проверка на получение сигнала от кого-нибудь
		if (Connect = accept(Listen, NULL, NULL))
		{
			std::cout << c_connect << ' ' << Count << std::endl;
			Connection[Count] = Client::Client(&Connect, Count);
			Count++;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)SendM, (LPVOID)(Count - 1), NULL, NULL);
		}
	}
	return 0;
}
