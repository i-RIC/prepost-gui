#include "gridattributestringconverterstring.h"

#include <QVariant>

GridAttributeStringConverterString::GridAttributeStringConverterString() :
	GridAttributeStringConverter {}
{}

QString GridAttributeStringConverterString::convert(const QVariant& value)
{
	return value.toString();
}
