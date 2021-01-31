#include "gridattributestringconverterreal.h"

#include <QString>
#include <QVariant>

GridAttributeStringConverterReal::GridAttributeStringConverterReal() :
	GridAttributeStringConverter {}
{}

QString GridAttributeStringConverterReal::convert(const QVariant& value)
{
	return QString::number(value.toDouble(), 'g', 10);
}
