#ifndef IRICLIB_BSTREAM_H
#define IRICLIB_BSTREAM_H

#include <iostream>
#include <string>

namespace iRICLib
{

class InputBStream {
public:
		InputBStream(std::istream& stream);
		InputBStream& operator >> (int& value);
		InputBStream& operator >> (double& value);
		InputBStream& operator >> (std::string& value);

private:
		std::istream* m_stream;
};

class OutputBStream {
public:
		OutputBStream(std::ostream& stream);
		OutputBStream& operator << (int value);
		OutputBStream& operator << (double value);
		OutputBStream& operator << (const std::string& value);

private:
		std::ostream* m_stream;
};

}


#endif // IRICLIB_BSTREAM_H
