#pragma once
#define MAXSIZE 500
struct PacketHeader
{
	int	pSize;
	int pOpcode;
};
struct Packet
{
	PacketHeader header;
	char Content[MAXSIZE];
};
enum Opcode 
{
	REQUSET_TIME = 1,
	REQUEST_NAME = 2,
	REQUEST_LIST = 3,
	REQUSET_MESSAGE = 4,
	SEND_TIME = 5,
	SNET_NAME = 6,
	SEND_LIST = 7,
	SEND_MESSAGE =8
};
// 时间记录用struct tm
// 名字用 char[MAXSIZE]
struct NameList
{
	int id;
	unsigned short port;
	struct in_addr;
};
struct Message
{
	int id;
	char Content[MAXSIZE];
};