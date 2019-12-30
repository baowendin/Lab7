#pragma once
#include <cstdint>
#include <iostream>
#include <string>

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

	template<typename T>
	void write(const T& x) {
		assert_enough(sizeof(T));
		*(T*)(buffer + head) = x;
		head += sizeof(T);
	}

	template<typename T>
	void write<std::vector<T>>(std::vector<T>& v)
	{
		write(static_cast<int>(v.size()));
		for (auto& t : v)
			write(t);
	}

	template<>
	void write<std::string>(const std::string& x) {
		write<int>(x.length());
		int len = x.length();
		assert_enough(len);
		memcpy(buffer + head, x.data(), len);
		head += len;
	}

	int length() const
	{
		return head;
	}
};

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

	template<typename T>
	void read(T& x) {
		assert_enough(sizeof(T));
		x = *(T*)(buffer + head);
		head += sizeof(T);
	}

	template<typename T>
	void read<std::vector<T>>(std::vector<T>& v)
	{
		v.clear();

		write(static_cast<int>(v.size()));
		for (auto& t : v)
			write(t);
	}

	template<>
	void read<std::string>(std::string& x) {
		int len;
		read(len);

		assert_enough(len);
		x.resize(len, 0);
		memcpy(x.data(), buffer + head, len);
		head += len;
	}

	int length() const
	{
		return head;
	}
};