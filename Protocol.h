#pragma once
#include <stdint.h>
#include <winsock.h>

struct Packet
{
	int size;
	uint8_t content[0];
};
struct ContentWithOp
{
	int opcode;
	uint8_t content[0];
};
enum Opcode : int
{
	REQUSET_TIME = 1,
	REQUEST_NAME = 2,
	REQUEST_LIST = 3,
	REQUSET_MESSAGE = 4,
	SEND_TIME = 5,
	SENT_NAME = 6,
	SEND_LIST = 7,
	SEND_MESSAGE =8
};
// ʱ���¼��struct tm
struct NameFormat
{
	int length;
	uint8_t name[0];
};

// �б��ʽ
struct ListRequestFormatContent
{
	int length;
	uint8_t* content[]; // һ�� ListItemFormat 
};
struct ListItemFormat
{
	int id;
	unsigned short port;
	struct in_addr addr;
};

// ��������Ϣ��ʽ
struct MessageRequestContent
{
	int receiver_id;
	int message_length;
	uint8_t content[0];
};

// ������Ϣ��ʽ
struct MessageSendContent
{
	int message_length;
	uint8_t content[0]; //��Ϣ����
};
