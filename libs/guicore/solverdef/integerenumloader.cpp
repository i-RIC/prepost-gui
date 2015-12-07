#include "integerenumloader.h"

IntegerEnumLoader::~IntegerEnumLoader()
{}

int IntegerEnumLoader::fromStr(const QString& str) const
{
	return str.toInt();
}
