#include "realenumloader.h"

RealEnumLoader::~RealEnumLoader()
{}

double RealEnumLoader::fromStr(const QString& str) const
{
	return str.toDouble();
}
