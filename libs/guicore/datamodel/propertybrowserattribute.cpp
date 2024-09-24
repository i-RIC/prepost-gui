#include "propertybrowserattribute.h"

PropertyBrowserAttribute::PropertyBrowserAttribute(const QString& n) :
	name {n},
	m_valueType {ValueType::Double},
	m_doubleValue {0},
	m_intValue {0},
	m_stringValue {}
{}

PropertyBrowserAttribute::PropertyBrowserAttribute(const QString& n, double val) :
	PropertyBrowserAttribute {n}
{
	m_valueType = ValueType::Double;
	m_doubleValue = val;
}

PropertyBrowserAttribute::PropertyBrowserAttribute(const QString& n, int val) :
	PropertyBrowserAttribute {n}
{
	m_valueType = ValueType::Int;
	m_intValue = val;
}

PropertyBrowserAttribute::PropertyBrowserAttribute(const QString& n, const QString& val) :
	PropertyBrowserAttribute {n}
{
	m_valueType = ValueType::String;
	m_stringValue = val;
}

PropertyBrowserAttribute::PropertyBrowserAttribute(const QString& n, const QVariant& val) :
	PropertyBrowserAttribute {n}
{
	bool ok = false;
	double dbl = val.toDouble(&ok);
	if (! ok) {
		m_valueType = ValueType::String;
		m_stringValue = val.toString();
	} else {
		m_valueType = ValueType::Double;
		m_doubleValue = dbl;
	}
}

QString PropertyBrowserAttribute::valueString(int doubleDecimals) const
{
	if (m_valueType == ValueType::Double) {
		if (std::fabs(m_doubleValue) < 1.0E-6) {
			return QString("%1").arg(m_doubleValue, 0, 'g', 10);
		} else {
			return QString("%1").arg(m_doubleValue, 0, 'f', doubleDecimals);
		}
	} else if (m_valueType == ValueType::Int) {
		return QString::number(m_intValue);
	} else if (m_valueType == ValueType::String) {
		return m_stringValue;
	}

	return "";
}
