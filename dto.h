#pragma once
#include "binary.h"
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
struct GetListRequest : ISerializable {};
struct GetListResponse : ISerializable {
    struct item
    {
        int id;
        // ¶Ë¿Ú
        // µØÖ·
    };
    std::vector<item> v;
    void serialize(BinaryWriter& writer)
    {
        writer.write(v);     
    }
    void deserialize(BinaryReader& reader)
    {

    }

};