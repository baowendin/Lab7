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
        int length;
        std::string addr;
        int port;
        // ¶Ë¿Ú
        // µØÖ·
    };
    int size;
    std::vector<ListItem> v;
};
SERIALIZER4(GetListResponse::ListItem, id, length, addr, port);
SERIALIZER2(GetListResponse, size, v);

struct GetMessageRequest
{
    int id;
    int length;
    std::string str;

};
SERIALIZER3(GetMessageRequest, id, length, str);

struct GetMessageResponse
{
    int length;
    std::string str;

};
SERIALIZER2(GetMessageResponse, length, str);

struct GetReturnInfo
{
    bool is_send;
};
SERIALIZER1(GetReturnInfo, is_send);