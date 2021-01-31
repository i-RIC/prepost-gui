#include "gridattributestringconverterenumerate.h"

GridAttributeStringConverterEnumerate::GridAttributeStringConverterEnumerate() :
	GridAttributeStringConverter {}
{}

void GridAttributeStringConverterEnumerate::setEnumerations(const QMap<QVariant, QString>& enums)
{
	m_enumerations = enums;
}

QString GridAttributeStringConverterEnumerate::convert(const QVariant& value)
{
	if (m_enumerations.find(value) == m_enumerations.end()) {
		return "";
	} else {
		return m_enumerations.value(value);
	}
}
