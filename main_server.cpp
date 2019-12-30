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
public:
    const char* name = "test_name";
    map<int, Client> hashmap;
   
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

    void work()
    {
        int count = 0;
        while (1)
        {            
            count++;
            SOCKET new_client_socket = accept(server_socket, NULL, NULL);
            cout << "服务器出来接客啦！这是第"<<count<<"个!"<<endl;
            Client temp_client;
            temp_client.socket = new_client_socket;
            temp_client.thread = new thread(thread_entry, this, new_client_socket);
            hashmap.insert(pair<int, Client>(count, temp_client));
        }
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
GetListResponse handle_request(Server* server,GetListRequest req)
{
    GetListResponse response;
    for (auto& v : server->hashmap)
    {
        ListItem item;
        item.id = v.first;
        SOCKADDR_IN sockAddr;
        int iLen = sizeof(sockAddr);
        getpeername(v.second.socket, (struct sockaddr*) & sockAddr, &iLen);//得到远程IP地址和端口号  注意函数参数1：此处是接受连接                                                                                                                                                                                  //socket
        item.addr = inet_ntoa(sockAddr.sin_addr);//IP 
        item.port = sockAddr.sin_port;       
        response.v.push_back(item);
    }
    return response;
}
GetReturnInfo handle_request(Server* server, GetMessageRequest req)
{
    GetMessageResponse response;
    GetReturnInfo info;
    
    if (server->hashmap.find(req.id) != server->hashmap.end())
    {
        info.is_send = false;
        return info;
    }
    info.is_send = true;
    response.length = req.length;
    response.str = std::move(req.str);
    SOCKET socket = server->hashmap.at(req.id).socket;
    send_to_user(socket, response);
    return info;
}

void send_to_user(SOCKET socket, GetMessageResponse response)
{
    uint8_t resp_buffer[2048];
    BinaryWriter writer(resp_buffer, 2048);
    writer.write((int)0); // 注意：等会再写回长度
    writer.write((int)Opcode::GET_MESSAGE);
    response.serialize(writer);
    *(int*)resp_buffer = writer.length();
    send(socket, (char*)resp_buffer, writer.length(), 0);
}

#define OPCODE_CASE(op, req_type) case Opcode::##op##: { \
		req_type req; \
		reader.read(req); \
		auto resp = handle_request(server, req); \
        writer.write(resp); \
		break; }

void thread_entry(Server* server, SOCKET socket)
{
    uint8_t buffer[10000];
    while (1)
    {
        // TODO: make this better (see main_client.cpp)
        // TODO: 包格式有改动
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
        auto packet = (Packet*)buffer;
        BinaryReader reader(packet->content, packet->total_size);

        uint8_t resp_buffer[2048];
        BinaryWriter writer(resp_buffer, 2048);
        writer.write((int)0); // 注意：等会再写回长度
        if (packet->op == Opcode::GET_MESSAGE)
        {
            writer.write((int)Opcode::GET_RETURN);
        }
        else writer.write((int)packet->op);

        // 具体的请求处理在上面handle_request
        switch (packet->op) {
            OPCODE_CASE(GET_TIME, GetTimeRequest);
            OPCODE_CASE(GET_NAME, GetNameRequest);
            OPCODE_CASE(GET_LIST, GetListRequest);
            OPCODE_CASE(GET_MESSAGE, GetMessageRequest);
            default: {
                cout << "不认识的Opcode：" << (int)packet->op << endl;
                return; // disconnect
            }
        }
        // 写回长度       
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
