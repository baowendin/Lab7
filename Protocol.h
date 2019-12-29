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