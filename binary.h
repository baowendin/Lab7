#pragma once
#include <cstdint>
#include <iostream>

class BinaryWriter
{
	uint8_t* buffer;
	int size;
	int head = 0;
public:
	BinaryWriter(uint8_t* buffer, int size) : buffer(buffer), size(size) {};

	void write(int x)
	{
		*(head + buffer) = x;
		head += sizeof(int);
	}

	void write(time_t time)
	{
		*(head + buffer) = time;
		head += sizeof(int);
	}
};

class BinaryReader
{
	uint8_t* buffer;
	int size;
	int head = 0;
public:
	BinaryReader(uint8_t* buffer, int size) : buffer(buffer), size(size) {};

	void read(int* x)
	{
		*x = *(buffer + head);
		head += sizeof(int);
	}

	void read(time_t* time)
	{
		*time = *(buffer + head);
		head += sizeof(time_t);
	}
};