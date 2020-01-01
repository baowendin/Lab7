#pragma once
#include <stdint.h>
#include <winsock.h>

enum class Opcode : int
{
    GET_TIME = 1, // ����/��Ӧʱ��
    GET_NAME = 2, // ����/��Ӧ����
    GET_LIST = 3, // ����/��Ӧ�����б�
    SEND_MESSAGE = 4, // ����/��Ӧ������Ϣ
    RECV_MESSAGE = 8, // �յ���Ϣ
};

struct Packet
{
    static const int HEADER_SIZE = 8;
    int total_size; // ����ͷ
    Opcode op;
    uint8_t content[0];
};