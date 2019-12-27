#pragma once
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