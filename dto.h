#pragma once
#include "binary.h"
#include <vector>
#include <winsock.h>

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
        // ¶Ë¿Ú
        // µØÖ·
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