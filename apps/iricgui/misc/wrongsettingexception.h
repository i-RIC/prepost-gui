#ifndef WRONGSETTINGEXCEPTION_H
#define WRONGSETTINGEXCEPTION_H

#include <misc/stringtool.h>
#include <stdexcept>

class WrongSettingException : public std::runtime_error
{
public:
	WrongSettingException(const QString& what) :
		std::runtime_error(iRIC::toStr(what))
	{}
};

#endif // WRONGSETTINGEXCEPTION_H

