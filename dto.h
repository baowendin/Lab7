#pragma once
#include "binary.h"
#include <winsock.h>
#include<vector>
class ISerializable {
public:
    virtual void serialize(BinaryWriter& writer) {}
    virtual void deserialize(BinaryReader& reader) {}
};

struct GetTimeRequest : ISerializable {};

struct GetTimeResponse : ISerializable {
    time_t time;
    void serialize(BinaryWriter& writer) {
        writer.write(time);
    }
    void deserialize(BinaryReader& reader) {
        reader.read(time);
    }
};

struct GetNameRequest : ISerializable {};

struct GetNameResponse : ISerializable {
    std::string name;
    void serialize(BinaryWriter& writer) {
        writer.write(name);
    }
    void deserialize(BinaryReader& reader) {
        reader.read(name);
    }
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