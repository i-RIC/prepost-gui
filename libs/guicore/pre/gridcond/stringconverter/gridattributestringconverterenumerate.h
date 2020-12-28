#ifndef GRIDATTRIBUTESTRINGCONVERTERENUMERATE_H
#define GRIDATTRIBUTESTRINGCONVERTERENUMERATE_H

#include "../../../guicore_global.h"

#include <QMap>
#include <QString>
#include <QVariant>

#include "../base/gridattributestringconverter.h"

class GUICOREDLL_EXPORT GridAttributeStringConverterEnumerate : public GridAttributeStringConverter
{
public:
	GridAttributeStringConverterEnumerate();

	void setEnumerations(const QMap<QVariant, QString>& enums);

	QString convert(const QVariant& value) override;

private:
	QMap<QVariant, QString> m_enumerations;
};

#endif // GRIDATTRIBUTESTRINGCONVERTERENUMERATE_H
