#pragma once
#include "binary.h"
#include <vector>
#include <winsock.h>

// 应用层协议
// 定义了各种请求、响应的具体内容及其序列化方式

struct GetTimeRequest {};
SERIALIZER0(GetTimeRequest);

struct GetTimeResponse {
    time_t time;
};
SERIALIZER1(GetTimeResponse, time);


struct GetNameRequest {};
SERIALIZER0(GetNameRequest);

struct GetNameResponse {
    std::string name;
};
SERIALIZER1(GetNameResponse, name);


struct GetListRequest {};
SERIALIZER0(GetListRequest);

struct GetListResponse {
    struct ListItem
    {
        int id;
        std::string addr;
        int port;
        // 端口
        // 地址
    };
    std::vector<ListItem> items;
};
SERIALIZER3(GetListResponse::ListItem, id, addr, port);
SERIALIZER1(GetListResponse, items);


struct SendMessageRequest
{
    int id;
    std::string str;
};
SERIALIZER2(SendMessageRequest, id, str);

struct SendMessageResponse
{
    bool is_sent;
};
SERIALIZER1(SendMessageResponse, is_sent);

struct ReceivedMessage
{
    std::string str;
};
SERIALIZER1(ReceivedMessage, str);