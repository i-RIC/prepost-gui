#include "gridattributestringconverterinteger.h"

#include <QVariant>

GridAttributeStringConverterInteger::GridAttributeStringConverterInteger() :
	GridAttributeStringConverter {}
{}

QString GridAttributeStringConverterInteger::convert(const QVariant& value)
{
	return value.toString();
}
