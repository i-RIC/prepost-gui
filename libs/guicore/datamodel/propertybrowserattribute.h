#ifndef PROPERTYBROWSERATTRIBUTE_H
#define PROPERTYBROWSERATTRIBUTE_H

#include "../guicore_global.h"

#include <QString>
#include <QVariant>

class GUICOREDLL_EXPORT PropertyBrowserAttribute
{

public:
	PropertyBrowserAttribute(const QString& n, double val);
	PropertyBrowserAttribute(const QString& n, int val);
	PropertyBrowserAttribute(const QString& n, const QString& val);
	PropertyBrowserAttribute(const QString& n, const QVariant& val);

	QString name;
	QVariant value;
};

#endif // PROPERTYBROWSERATTRIBUTE_H
