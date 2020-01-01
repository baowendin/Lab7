#include <iostream>
#include <winsock.h>
#include <vector>
#include <unordered_map>
#include <thread>
#include <functional>
#include <atomic>
#include <ctime>
#include "protocol.h"
#include "dto.h"

#pragma comment(lib,"ws2_32.lib")

using namespace std;

class InteractiveClient {
    thread _receive_thread;
    atomic<bool> _stop_receiving = false;
    atomic<bool> _connected = false;
    SOCKET _socket;

    template<typename T>
    void send_request(Opcode op, const T& req) {
        uint8_t buffer[2048];
        BinaryWriter writer(buffer, sizeof(buffer));
        writer.write((int)0);
        writer.write((int)op);
        writer.write(req);
        *(int*)buffer = writer.length();
        send(_socket, (char*)buffer, writer.length(), 0);
    }

    void on_received(Opcode op, vector<char> received) {
        printf("SERVER RESPONSE RECEIVED\n");
        BinaryReader reader((uint8_t*)received.data(), received.size());
        switch (op) {
            case Opcode::GET_TIME: {
                GetTimeResponse resp;
                reader.read(resp);

                tm* t = localtime(&resp.time);
                char time_str[128];
                strftime(time_str, sizeof(time_str), "%a, %d.%m.%Y %H:%M:%S", t);
                printf("time: %s\n", time_str);
                break;
            }

            case Opcode::GET_NAME: {
                GetNameResponse resp;
                reader.read(resp);

                printf("name: %s\n", resp.name.c_str());
                break;
            }

            case Opcode::GET_LIST: {
                GetListResponse resp;
                reader.read(resp);

                printf("server connection list:\n");
                for (auto& item : resp.items) {
                    printf("id=%d, addr=%s, port=%d\n", item.id, item.addr.c_str(), item.port);
                }
                break;
            }

            case Opcode::RECV_MESSAGE: {
                ReceivedMessage recv;
                reader.read(recv);

                printf("message from another client:\n");
                printf("%s\n", recv.str.c_str());
                break;
            }

            case Opcode::SEND_MESSAGE: {
                SendMessageResponse resp;
                reader.read(resp);
                printf("send %s\n", resp.is_sent ? "ok" : "failed");
                break;
            }

            default: {
                printf("UNEXPECTED RESPONSE\n");
            }
        }
    }

    void receive_entry() {
        vector<char> buffer; // 总缓冲区
        const int RECV_BUF_SIZE = 2048;
        char recv_buf[RECV_BUF_SIZE]; // 调用系统recv的缓冲区
        while (!_stop_receiving) {
            int amt = recv(_socket, recv_buf, RECV_BUF_SIZE, 0);
            if (amt == -1) break;
            size_t cur = buffer.size();
            buffer.resize(cur + amt); // 扩大总缓冲区
            memcpy(buffer.data() + cur, recv_buf, amt); // 复制进总缓冲区

            // 可能含有多个包，直到所有包都被消耗
            while (buffer.size() >= Packet::HEADER_SIZE) {
                auto packet = (Packet*)buffer.data();
                int total_length = packet->total_size;
                int content_length = total_length - Packet::HEADER_SIZE;
                Opcode op = packet->op;

                // 长度>=包头指示的长度，代表这个包已经足够
                if (buffer.size() >= total_length) {
                    vector<char> received;
                    received.resize(content_length);
                    memcpy(received.data(), buffer.data() + Packet::HEADER_SIZE, content_length);
                    buffer.erase(buffer.begin(), buffer.begin() + total_length);
                    on_received(op, move(received)); // 处理包内容
                }
                else {
                    break;
                }
            }
        }
        printf("接收线程退出\n");
        _connected = false;
    }

public:
    InteractiveClient() {
        /* 加载套接字库 */
        WORD w_req = MAKEWORD(2, 2);//版本号
        WSADATA wsadata;
        int err = WSAStartup(w_req, &wsadata);
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

    void connect_server() {
        if (_connected) {
            printf("already connected\n");
            return;
        }
        _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (_socket == INVALID_SOCKET) {
            printf("socket initialization failed\n");
        }

        // 端口8080
        const int port = 8080;
        hostent* host;
        if ((host = gethostbyname("localhost")) == NULL)
        {
            printf("failed to resolve hostname\n");
            WSACleanup();
            return;
        }

        SOCKADDR_IN sock_addr;
        sock_addr.sin_port = htons(port);
        sock_addr.sin_family = AF_INET;
        sock_addr.sin_addr.s_addr = *((unsigned long*)host->h_addr);

        if (connect(_socket, (SOCKADDR*)(&sock_addr), sizeof(sock_addr)) != 0) {
            printf("failed to establish connection with server\n");
            WSACleanup();
            return;
        }

        _receive_thread = thread([&]() { receive_entry(); });
        _connected = true;
    }

    void disconnect_server() {
        if (!_connected) {
            printf("not connected\n");
            return;
        }

        closesocket(_socket);

        _stop_receiving = true;
        _receive_thread.join();
        _stop_receiving = false;
        _connected = false;
    }

    void get_time() {
        send_request(Opcode::GET_TIME, GetTimeRequest());
    }

    void get_name() {
        send_request(Opcode::GET_NAME, GetNameRequest());
    }

    void get_list() {
        send_request(Opcode::GET_LIST, GetListRequest());
    }

    void send_message() {
        SendMessageRequest req;
        int id;
        string msg;
        printf("Receiver? (connection id)\n");
        cin >> id;
        printf("Message?\n");
        cin >> msg;
        req.id = id;
        req.str = move(msg);
        send_request(Opcode::SEND_MESSAGE, req);
    }
};


struct Action {
    string description;
    function<void()> handler;
};

unordered_map<int, Action> actions;

void register_action(string description, function<void()> handler) {
    actions.insert(make_pair(actions.size() + 1, Action{ description, handler }));
}

int main() {

    // 客户端
    InteractiveClient client;

    // 注册各个操作
    register_action("connect server", [&]() { client.connect_server(); });
    register_action("disconnect server", [&]() { client.disconnect_server(); });
    register_action("get server time", [&]() { client.get_time(); });
    register_action("get server name", [&]() { client.get_name(); });
    register_action("list connection", [&]() { client.get_list(); });
    register_action("send message", [&]() { client.send_message(); });

    while (true) {
        // 打印菜单
        char menu_buf[1024] = { 0 };
        char* p = menu_buf;
        p += sprintf(p, "\n");
        p += sprintf(p, "=============================\n");
        p += sprintf(p, "Menu\n");
        for (auto [op, action] : actions) {
            p += sprintf(p, "%d. %s\n", op, action.description.c_str());
        }
        p += sprintf(p, "=============================\n");
        printf("%s", menu_buf);

        int op = -1;
        cin >> op;
        if (auto iter = actions.find(op); iter != actions.end()) {
            auto [op, action] = *iter;
            action.handler(); // 执行具体的操作
        }
        else {
            printf("this operation \"%d\" is not present\n", op);
        }
    }
}