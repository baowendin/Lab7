#pragma once
#include <stdint.h>
#include <winsock.h>

enum class Opcode : int
{
    GET_TIME = 1,
    GET_NAME = 2,
    GET_LIST = 3,
    GET_MESSAGE = 4,
    GET_RETURN = 5
    //REQUSET_TIME = 1,
    //REQUEST_NAME = 2,
    //REQUEST_LIST = 3,
    //REQUSET_MESSAGE = 4,
    //SEND_TIME = 5,
    //SENT_NAME = 6,
    //SEND_LIST = 7,
    //SEND_MESSAGE = 8
};

struct Packet
{
    static const int HEADER_SIZE = 8;
    int total_size; // °üÀ¨Í·
    Opcode op;
    uint8_t content[0];
};