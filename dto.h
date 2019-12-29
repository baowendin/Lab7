#pragma once
#include "binary.h"

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