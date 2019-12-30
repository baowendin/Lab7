#pragma once
#include "binary.h"
#include <vector>

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
    struct item
    {
        int id;
        // ¶Ë¿Ú
        // µØÖ·
    };

    std::vector<item> v;
};
SERIALIZER1(GetListResponse::item, id);
SERIALIZER1(GetListResponse, v);