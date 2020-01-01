#pragma once
#include <stdint.h>
#include <winsock.h>

enum class Opcode : int
{
    GET_TIME = 1, // 请求/响应时间
    GET_NAME = 2, // 请求/响应名称
    GET_LIST = 3, // 请求/响应连接列表
    SEND_MESSAGE = 4, // 请求/响应发送消息
    RECV_MESSAGE = 8, // 收到消息
};

struct Packet
{
    static const int HEADER_SIZE = 8;
    int total_size; // 包括头
    Opcode op;
    uint8_t content[0];
};