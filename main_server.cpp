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

class Server;

void thread_entry(Server* server, SOCKET socket);

class Server
{
private:
    SOCKET server_socket;
    struct sockaddr_in server_addr;
    map<int, Client> hashmap;
public:
    const char* name = "test_name";
   
    Server()
    {
        server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (server_socket < 0)
        {
            perror("SeverSocket Build Error: ");
            return;
        }
        int optval = 1;
        /* ��Ҳ��֪�������ʲô�õ�
        if (setsockopt(ServerSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(int)) < 0)
            perror("Set Socket Error:");
        */
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(8080);
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        bind(server_socket, (struct sockaddr*) & server_addr, sizeof(server_addr));
        listen(server_socket, MAXCLIENT);
    }

    void work()
    {
        int count = 0;
        while (1)
        {            
            count++;
            SOCKET new_client_socket = accept(server_socket, NULL, NULL);
            cout << "�����������ӿ��������ǵ�"<<count<<"��!"<<endl;
            Client temp_client;
            temp_client.socket = new_client_socket;
            temp_client.thread = new thread(thread_entry, this, new_client_socket);
            hashmap.insert(pair<int, Client>(count, temp_client));
        }
    }
};

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

GetNameResponse handle_request(Server* server, GetNameRequest req) {
    // TODO   
    GetNameResponse response;
    response.name = "kami";
    return response;
}

GetTimeResponse handle_request(Server* server, GetTimeRequest req) {
    // TODO
    GetTimeResponse response;
    response.time = time(0);
    return response;
}
/*GetListResponse handle_request(Server* server,GetListRequest req)
{

}*/
#define OPCODE_CASE(op, req_type) case Opcode::##op##: { \
		req_type req; \
		req.deserialize(reader); \
		handle_request(server, req).serialize(writer); \
		break; }

void thread_entry(Server* server, SOCKET socket)
{
    uint8_t buffer[10000];
    while (1)
    {
        // TODO: make this better (see main_client.cpp)
        // TODO: ����ʽ�иĶ�
        int tot_size = 0;
        while (tot_size < sizeof(int))
        {
            int size = recv(socket, (char*)buffer + tot_size, sizeof(int) - tot_size, 0);
            if (size == 0)
            {
                cout << "�ͻ��˹ر�" << endl;
                return;
            }
            tot_size += size;
        }
        int* size = static_cast<int*>((void*)buffer);
        while (tot_size < *size)
        {
            int tmp_size = recv(socket, (char*)buffer + tot_size, *size - tot_size, 0);
            if (tmp_size == 0)
            {
                cout << "�ͻ��˹ر�" << endl;
                return;
            }
            tot_size += tmp_size;
        }


        auto packet = (Packet*)buffer;
        BinaryReader reader(packet->content, packet->total_size);

        uint8_t resp_buffer[2048];
        BinaryWriter writer(resp_buffer, 2048);
        writer.write((int)0); // ע�⣺�Ȼ���д�س���
        writer.write((int)packet->op);

        // �����������������handle_request
        switch (packet->op) {
            OPCODE_CASE(GET_TIME, GetTimeRequest);
            OPCODE_CASE(GET_NAME, GetNameRequest);
            default: {
                cout << "����ʶ��Opcode��" << (int)packet->op << endl;
                return; // disconnect
            }
        }

        // д�س���
        *(int*)resp_buffer = writer.length();
        send(socket, (char*)resp_buffer, writer.length(), 0);
    }
}

int main()
{
    initialize();
    Server server;
    server.work();

}
