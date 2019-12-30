#pragma once
#include "binary.h"
#include <vector>

// this looks evil
// maybe should not be used
#define SERIALIZER(type, ser, deser) template<> struct Serializer<type> { \
    void serialize(BinaryWriter& bw, const type& obj) { do ser while(0); } \
    void deserialize(BinaryReader& br, const type& obj) { do deser while (0); } \
};

#define FIELD0(p, fld)
#define FIELD1(p, fld) p(obj.##fld);
#define FIELD2(p, fld, ...) _CONCAT(FIELD1(p, fld), FIELD1(p, __VA_ARGS__))
#define FIELD3(p, fld, ...) _CONCAT(FIELD1(p, fld), FIELD2(p, __VA_ARGS__))
#define FIELD4(p, fld, ...) _CONCAT(FIELD1(p, fld), FIELD3(p, __VA_ARGS__))

#define _SERIALIZERN(n, type, ...) SERIALIZER(type, { FIELD##n(bw.write, __VA_ARGS__) }, { FIELD##n(br.read, __VA_ARGS__) })
#define SERIALIZER0(type, ...) _SERIALIZERN(0, type, __VA_ARGS__)
#define SERIALIZER1(type, ...) _SERIALIZERN(1, type, __VA_ARGS__)
#define SERIALIZER2(type, ...) _SERIALIZERN(2, type, __VA_ARGS__)
#define SERIALIZER3(type, ...) _SERIALIZERN(2, type, __VA_ARGS__)
#define SERIALIZER4(type, ...) _SERIALIZERN(2, type, __VA_ARGS__)


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

struct ListItem
{
    int id;
    char* addr;
    int port;
    // ¶Ë¿Ú
    // µØÖ·
};

struct GetListRequest : ISerializable {};
struct GetListResponse : ISerializable {

    std::vector<ListItem> v;
    void serialize(BinaryWriter& writer)
    {
        writer.write(v);     
    }
    void deserialize(BinaryReader& reader)
    {

    }

};

struct GetMessageRequest : ISerializable
{
    int id;
    int length;
    std::string str;
    void serialize(BinaryWriter& writer)
    {
    }
    void deserialize(BinaryReader& reader)
    {

    }
};

struct GetMessageResponse : ISerializable
{
    int length;
    std::string str;
    SOCKET socket;
    void serialize(BinaryWriter& writer)
    {
    }
    void deserialize(BinaryReader& reader)
    {

    }
};

struct GetReturnInfo : ISerializable 
{
    bool is_send;
};