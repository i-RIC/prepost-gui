#include "iriclib_bstream.h"
#include <stdlib.h>
#include <string.h>

using namespace iRICLib;

InputBStream::InputBStream(std::istream &stream)
{
	m_stream = &stream;
}

InputBStream& InputBStream::operator >> (int& value)
{
	int val;
	m_stream->read(reinterpret_cast<char*>(&val), sizeof(int));
	value = val;
	return *this;
}

InputBStream& InputBStream::operator >> (double& value)
{
	double val;
	m_stream->read(reinterpret_cast<char*>(&val), sizeof(double));
	value = val;
	return *this;
}

InputBStream& InputBStream::operator >> (std::string& str)
{
	char* buffer;
	int size;
	m_stream->read(reinterpret_cast<char*>(&size), sizeof(int));
	buffer = new char[size];
	m_stream->read(buffer, sizeof(char) * size);

	str = buffer;

	delete buffer;
	return *this;
}

OutputBStream::OutputBStream(std::ostream &stream)
{
	m_stream = &stream;
}

OutputBStream& OutputBStream::operator << (int value)
{
	m_stream->write(reinterpret_cast<char*>(&value), sizeof(int));
	return *this;
}

OutputBStream& OutputBStream::operator << (double value)
{
	m_stream->write(reinterpret_cast<char*>(&value), sizeof(double));
	return *this;
}

OutputBStream& OutputBStream::operator << (const std::string& str)
{
	const char* cstr = str.c_str();
	size_t size = strlen(cstr) + 1;
	m_stream->write(reinterpret_cast<char*>(&size), sizeof(int));
	m_stream->write(const_cast<char*>(cstr), sizeof(char) * size);

	return *this;
}
