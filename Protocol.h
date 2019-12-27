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
// 时间记录用struct tm
struct NameFormat
{
	int length;
	uint8_t name[0];
};

// 列表格式
struct ListRequestFormatContent
{
	int length;
	uint8_t* content[]; // 一堆 ListItemFormat 
};
struct ListItemFormat
{
	int id;
	unsigned short port;
	struct in_addr addr;
};

// 请求发送信息格式
struct MessageRequestContent
{
	int receiver_id;
	int message_length;
	uint8_t content[0];
};

// 发送信息格式
struct MessageSendContent
{
	int message_length;
	uint8_t content[0]; //消息内容
};
