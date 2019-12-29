#pragma once
#include "binary.h"

class ISerializable
{
public:
	virtual void serialize(BinaryWriter& writer) = 0;
	virtual void deserialize(BinaryReader& reader) = 0;
};

class SendTime : ISerializable
{
	time_t time;
public:
	SendTime(time_t time)
	{
		this->time = time;
	}
	SendTime() {}
	void serialize(BinaryWriter& writer)
	{
		writer.write(time);
	}
	void deserialize(BinaryReader& reader)
	{
		reader.read(time);
	}
};
class SendName : ISerializable
{
	std::string name;
	int length;
public:
	SendName(std::string str)
	{
		name = str;
		length = str.length();
	}
	SendName(){}
	void serialize(BinaryWriter& writer)
	{
		writer.write(length);
		for (int i = 0; i < length; i++)
		{
			writer.write(name[i]);
		}
	}
	void deserialize(BinaryReader& reader)
	{
		reader.read(length);
		name = "";
		for (int i = 0; i < length; i++)
		{
			char c;
			reader.read(c);
			name += c;
		}
	}
	int get_size()
	{
		return sizeof(int) + sizeof(char) * length;
	}
};