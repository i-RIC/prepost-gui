#include "propertybrowserattribute.h"

PropertyBrowserAttribute::PropertyBrowserAttribute(const QString& n, double val)
{
	name = n;
	value = val;
}

PropertyBrowserAttribute::PropertyBrowserAttribute(const QString& n, int val)
{
	name = n;
	value = val;
}

PropertyBrowserAttribute::PropertyBrowserAttribute(const QString& n, const QString& val)
{
	name = n;
	value = val;
}
