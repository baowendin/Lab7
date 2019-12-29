#include <iostream>
#include <winsock.h>
#include <vector>
#include <map>
#include <thread>
#include "protocol.h"
#include "dto.h"

#pragma comment(lib,"ws2_32.lib")
#define MAXCLIENT 10
#define _CRT_SECURE_NO_WARNINGS
using namespace std;
struct Client
{
	SOCKET socket;
	thread* thread;
};
struct Message
{
	int size;
	int opcode;
	uint8_t content[0];
};
class Operation;
void Run(Server* server, SOCKET socket);
class Server
{
private:
	SOCKET server_socket;
	struct sockaddr_in server_addr;
	map<int, struct Client> hashmap;
public:
	static char name[20];
	Server()
	{
		server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (server_socket < 0)
		{
			perror("SeverSocket Build Error: ");
			return;
		}
		int optval = 1;
		/* 我也不知道这个干什么用的
		if (setsockopt(ServerSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(int)) < 0)
			perror("Set Socket Error:");
		*/
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(8080);
		server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
		bind(server_socket, (struct sockaddr*) & server_addr, sizeof(server_addr));
		listen(server_socket, MAXCLIENT);
	}
	void Work()
	{
		int count = 0;
		while (1)
		{
			count++;
			SOCKET new_client_socket = accept(server_socket, NULL, NULL);
			struct Client temp_client;
			hashmap.insert(pair<int, struct Client>(count, temp_client));
			temp_client.socket = new_client_socket;
			temp_client.thread = new thread(Run, this, new_client_socket);
		}
	}
};
class Operation
{
public:
	static uint8_t* to_buffer(int op,int& size)
	{
		time_t timep;
		time(&timep);
		SendTime send_time(timep);
		size = sizeof(int) * 2 + sizeof(time_t);
		struct Message* message = (struct Message*)malloc(size);
		message->size = sizeof(int) + sizeof(time_t);
		message->opcode = op;
		BinaryWriter writer(message->content, sizeof(time_t));
		send_time.serialize(writer);
		return static_cast<uint8_t*>((void*)message);
	}
	static uint8_t* to_buffer(int op, string name, int& size)
	{
		
		SendName send_name(name);
		size = sizeof(int) * 2 + send_name.get_size();
		struct Message* message = (struct Message*)malloc(size);
		message->size = send_name.get_size() + sizeof(int);
		message->opcode = op;
		BinaryWriter writer(message->content, message->size);
		send_name.serialize(writer);
		return static_cast<uint8_t*>((void*)message);
	}
	static uint8_t* to_buffer(int op, map<int, struct Client>* hashmap, int* size)
	{
		struct Message* message = (struct Message*)malloc(sizeof(int) * 3 + hashmap->size() * sizeof(ListItemFormat));
		message->size = sizeof(int) * 2 + hashmap->size() * sizeof(ListItemFormat);
		message->opcode = op;
		struct ListRequestFormatContent* list = static_cast<ListRequestFormatContent*>((void*)(message + sizeof(int) * 2));
		list->length = hashmap->size();
		map<int, Client>::iterator iter;
		iter = (*hashmap).begin();
		int bias = sizeof(int) * 3;
		while (iter != (*hashmap).end())
		{
			struct ListItemFormat* item = static_cast<ListItemFormat*>((void*)(message + sizeof(int) * 3));
			item->id = iter->first;
			SOCKET peer_socket = iter->second.socket;
			struct sockaddr_in peer_addr;
			int peerlen = sizeof(struct sockaddr_in);
			if (getpeername(peer_socket, (struct sockaddr*) & peer_addr, &peerlen) != 0)
			{
				cout << "获取远程地址错误" << endl;
			}
			item->port = peer_addr.sin_port;
			item->addr = peer_addr.sin_addr;
			bias += sizeof(ListItemFormat);
		}
		return static_cast<uint8_t*>((void*)message);
	}
	static void time_operation(SOCKET socket)
	{
		uint8_t* buffer;
		int size;
		buffer = to_buffer(Opcode::SEND_TIME,size);
		send(socket, static_cast<char*>((void*)buffer), size, 0);
	}
	static void name_operation(SOCKET socket)
	{
		uint8_t* buffer;
		int size;
		buffer = to_buffer(Opcode::SENT_NAME, Server::name,size);
		send(socket, static_cast<char*>((void*)buffer), size, 0);
	}
	static void list_operation(SOCKET socket, map<int, struct Client>* hashmap)
	{
		uint8_t* buffer;
		int* size = new int();
		buffer = to_buffer(Opcode::SEND_LIST, hashmap, size);
		send(socket, static_cast<char*>((void*)buffer), *size, 0);
	}
	static void message_operation(SOCKET socket, int* id)
	{
		uint8_t* buffer;
		int* size;

	}
};
void initialize()
{
	/* 加载套接字库 */
	WORD w_req = MAKEWORD(2, 2);//版本号
	WSADATA wsadata;
	int err;
	err = WSAStartup(w_req, &wsadata);
	if (err != 0) {
		cout << "初始化套接字库失败！" << endl;
	}
	else {
		cout << "初始化套接字库成功！" << endl;
	}
	//检测版本号
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
		cout << "套接字库版本号不符！" << endl;
		WSACleanup();
	}
	else {
		cout << "套接字库版本正确！" << endl;
	}
}
void Run(Server *server, SOCKET socket)
{
	int8_t buffer[10000];
	while (1)
	{
		int tot_size = 0;
		while (tot_size < sizeof(int))
		{
			int size = recv(socket, (char*)buffer + tot_size, sizeof(int) - tot_size, 0);
			if (size == 0)
			{
				cout << "客户端关闭" << endl;
				return;
			}
			tot_size += size;
		}
		int* size = static_cast<int*>((void*)buffer);
		tot_size = 0;
		while (tot_size < *size)
		{
			int tmp_size = recv(socket, (char*)buffer + tot_size, *size - tot_size, 0);
			if (tmp_size == 0)
			{
				cout << "客户端关闭" << endl;
				return;
			}
			tot_size += tmp_size;
		}
		struct ContentWithOp* content = static_cast<struct ContentWithOp*>((void*)buffer);
		switch (content->opcode)
		{
		case REQUSET_TIME:
			Operation::time_operation(socket);
			break;
		case REQUEST_NAME:
				Operation::name_operation(socket);
				break;
			/*case REQUEST_LIST:
				Operation::list_operation(socket, hashmap);
				break;
			case REQUSET_MESSAGE:
				Operation::message_operation(socket, static_cast<int*>((void*)content->content));
				break;*/
		default:
			cout << "Invalid selection\n";
		}
	}
}
int main()
{
	initialize();
	Server server;
	server.Work();

}
