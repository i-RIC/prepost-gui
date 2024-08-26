#ifndef PROPERTYBROWSERATTRIBUTE_H
#define PROPERTYBROWSERATTRIBUTE_H

#include "../guicore_global.h"

#include <QString>
#include <QVariant>

class GUICOREDLL_EXPORT PropertyBrowserAttribute
{

public:
	enum class ValueType {
		Double,
		Int,
		String
	};

	PropertyBrowserAttribute(const QString& n);
	PropertyBrowserAttribute(const QString& n, double val);
	PropertyBrowserAttribute(const QString& n, int val);
	PropertyBrowserAttribute(const QString& n, const QString& val);
	PropertyBrowserAttribute(const QString& n, const QVariant& val);

	QString valueString(int doubleDecimals) const;

	QString name;

	ValueType m_valueType;

	double m_doubleValue;
	int m_intValue;
	QString m_stringValue;
};

#endif // PROPERTYBROWSERATTRIBUTE_H
