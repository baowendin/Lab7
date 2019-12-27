#pragma once
#include <cstdint>
#include<iostream>
using namespace std;
class BinaryWriter
{
	uint8_t* buffer;
	int size;
	int head;
public:
	BinaryWriter(uint8_t* buffer, int size)
	{
		this->buffer = buffer;
		this->size = size;
		this->head = 0;
	}
	void Write(int x)
	{
		*(head + buffer) = x;
		head += sizeof(int);
	}
	void Write(time_t time)
	{
		*(head + buffer) = time;
		head += sizeof(int);
	}
	int get()
	{
		return *(head + buffer);
	}
};
class BinaryReader
{
	uint8_t* buffer;
	int size;
	int head;
public:
	BinaryReader(uint8_t* buffer, int size)
	{
		this->buffer = buffer;
		this->size = size;
		this->head = 0;
	}
	void Read(int* x)
	{
		*x = *(buffer + head);
		head += sizeof(int);
	}
	void Read(time_t* time)
	{
		*time = *(buffer + head);
		head += sizeof(time_t);
	}
};