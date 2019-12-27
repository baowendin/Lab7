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
class ISerializable
{
public:
	virtual void serialize(BinaryWriter writer) = 0;
	virtual void deserialize(BinaryReader reader) = 0;
};
class SendTime :ISerializable
{
	time_t time;
public:
	SendTime(time_t time)
	{
		this->time = time;
	}
	void serialize(BinaryWriter writer)
	{
		writer.Write(time);
	}
	void deserialize(BinaryReader reader)
	{
		reader.Read(&time);
	}

};
int main()
{
	uint8_t* buf = (uint8_t*)malloc(10 * sizeof(int));
	/* test read */
	BinaryWriter binary_writer(buf, 10 * sizeof(int));
	for (int i = 0; i < 10; i++)
	{
		binary_writer.Write(i);
	}
	BinaryReader binary_reader(buf, 10 * sizeof(int));
	for (int i = 0; i < 10; i++)
	{
		int* x = new int();
		binary_reader.Read(x);
		cout << *x << endl;
	}
}