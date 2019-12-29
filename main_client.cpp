#include <iostream>
#include <winsock.h>
#include <vector>
#include <unordered_map>
#include <thread>
#include <functional>
#include <atomic>
#include "protocol.h"
#include "dto.h"

#pragma comment(lib,"ws2_32.lib")


using namespace std;


class Protocol {
public:
	
};

class InteractiveClient {
	thread _receive_thread;
	atomic<bool> _stop_receiving = false;
	bool _connected = false;
	SOCKET _socket;

	void receive_entry() {
		vector<char> buffer;
		const int RECV_BUF_SIZE = 2048;
		char recv_buf[RECV_BUF_SIZE];
		while (!_stop_receiving) {
			int amt = recv(_socket, recv_buf, RECV_BUF_SIZE, 0);
			if (amt == 0) {
				break;
			}

			size_t cur = buffer.size();
			buffer.resize(cur + amt);
			memcpy(buffer.data() + cur, recv_buf, amt);

			// 头4字节指示长度，不包括头
			if (buffer.size() >= sizeof(int)) {
				// expect包括头
				int expect = *(int*)buffer.data() + sizeof(int);
				if (buffer.size() >= expect) {

				}
			}
		}
		printf("接收线程退出");
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

	InteractiveClient client;

	register_action("connect server", [&]() { client.connect_server(); });
	register_action("disconnect server", [&]() {client.disconnect_server(); });

	while (true) {
		printf("Menu\n");
		for (auto [op, action] : actions) {
			printf("%d. %s\n", op, action.description.c_str());
		}

		int op = -1;
		cin >> op;
		if (auto iter = actions.find(op); iter != actions.end()) {
			auto [op, action] = *iter;
			action.handler();
		}
		else {
			printf("this operation \"%d\" is not present\n", op);
		}
	}
}