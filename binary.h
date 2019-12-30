#pragma once
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

class BinaryWriter;
class BinaryReader;

// 如果要支持新类型，请进行偏特化（下方）
template<typename T>
struct Serializer {
    static_assert("No specialization of Serializer<T> provided");
    void serialize(BinaryWriter& bw, const T& t) {}
    void deserialize(BinaryReader& br, T& t) {}
};

// 二进制写入器
// 注意：支持基础类型、可序列化（实现了Serializer<T>）类型的直接写入
class BinaryWriter
{
    uint8_t* buffer;
    int size;
    int head = 0;

    void assert_enough(int l) {
        if (head + l > size) {
            throw std::out_of_range("no data remain");
        }
    }

public:
    BinaryWriter(uint8_t* buffer, int size) : buffer(buffer), size(size) {};

    // 基础类型
    template<typename T, std::enable_if_t<std::is_trivially_copyable<T>::value, bool>>
    void write(const T& x) {
        assert_enough(sizeof(T));
        *(T*)(buffer + head) = x;
        head += sizeof(T);
    }

    void write(void* src, int len) {
        assert_enough(len);
        memcpy(buffer + head, src, len);
        head += len;
    }

    // 可序列化类型
    template<typename T, std::size_t = sizeof(Serializer<T>)>
    void write(const T& t) {
        Serializer<T>().serialize(*this, t);
    }

    int length() const
    {
        return head;
    }
};

// 二进制读取器
// 注意：支持基础类型、可序列化（实现了Serializer<T>）类型的直接读取
class BinaryReader
{
    uint8_t* buffer;
    int size;
    int head = 0;

    void assert_enough(int l) {
        if (head + l > size) {
            throw std::out_of_range("no data remain");
        }
    }

public:
    BinaryReader(uint8_t* buffer, int size) : buffer(buffer), size(size) {};

    // 基础类型
    template<typename T, std::enable_if_t<std::is_trivially_copyable<T>::value, bool>>
    void read(T& x) {
        assert_enough(sizeof(T));
        x = *(T*)(buffer + head);
        head += sizeof(T);
    }

    void read(void* dest, int len) {
        assert_enough(len);
        memcpy(dest, buffer + head, len);
        head += len;
    }

    // 可序列化类型
    template<typename T, std::size_t = sizeof(Serializer<T>)>
    void read(T& t) {
        Serializer<T>().deserialize(*this, t);
    }

    int length() const
    {
        return head;
    }
};


// 支持string的序列化
template<>
struct Serializer<std::string> {
    void serialize(BinaryWriter& bw, const std::string& x) {
        int len = x.length();
        bw.write(len);
        bw.write((void*)x.data(), len);
    }

    void deserialize(BinaryReader& br, std::string& t) {
        int len;
        br.read(len);
        t.resize(len);
        br.read(t.data(), len);
    }
};

// 支持vector<T>的序列化
template<typename T>
struct Serializer<std::vector<T>> {
    void serialize(BinaryWriter& bw, const std::vector<T>& x) {
        int len = x.size();
        bw.write(len);
        for (auto& elem : x) {
            bw.write(elem);
        }
    }

    void deserialize(BinaryReader& br, std::vector<T>& x) {
        int len;
        br.read(len);
        x.resize(len);
        for (auto& elem : x) {
            br.read(x);
        }
    }
};


// 组合定义序列化器的辅助宏
// looks evil, but could save boilerplate code
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