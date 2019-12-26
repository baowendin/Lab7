#include<iostream>
#include<winsock.h>
#include <vector>
#include <thread>
#pragma comment(lib,"ws2_32.lib")
#define MAXCLIENT 10
using namespace std;
void initialize()
{
	/* �����׽��ֿ� */
	WORD w_req = MAKEWORD(2, 2);//�汾��
	WSADATA wsadata;
	int err;
	err = WSAStartup(w_req, &wsadata);
	if (err != 0) {
		cout << "��ʼ���׽��ֿ�ʧ�ܣ�" << endl;
	}
	else {
		cout << "��ʼ���׽��ֿ�ɹ���" << endl;
	}
	//���汾��
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
		cout << "�׽��ֿ�汾�Ų�����" << endl;
		WSACleanup();
	}
	else {
		cout << "�׽��ֿ�汾��ȷ��" << endl;
	}
}
void Run(SOCKET& socket, SOCKET* socketList)
{

}
class Server
{
private:
	SOCKET ServerSocket;
	SOCKET SocketList[MAXCLIENT];
	thread threadList[MAXCLIENT];
	struct sockaddr_in ServerAddr;
public:
	Server()
	{
		ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (ServerSocket < 0)
		{
			perror("SeverSocket Build Error: ");
			return;
		}
		int optval = 1;
		/* ��Ҳ��֪�������ʲô�õ�
		if (setsockopt(ServerSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(int)) < 0)
			perror("Set Socket Error:");
		*/
		ServerAddr.sin_family = AF_INET;
		ServerAddr.sin_port = htons(8080);
		ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		bind(ServerSocket, (struct sockaddr*) & ServerAddr, sizeof(ServerAddr));
		listen(ServerSocket, MAXCLIENT);
		SocketList[0] = 0;		
	}
	void Work()
	{
		while (1)
		{
			SOCKET client = accept(ServerSocket, NULL, NULL);
			SocketList[0]++;
			SocketList[SocketList[0]] = client;
			thread t(Run, ref(client), (SOCKET*)&SocketList);
		}
	}
};
int main()
{
	initialize();
	Server server;
	server.Work();
	//SOCKET SeverSocket = socket(AF_INET, SOCK_STREAM, 0);
	
}