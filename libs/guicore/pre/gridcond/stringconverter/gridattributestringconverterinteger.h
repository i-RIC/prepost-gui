#ifndef GRIDATTRIBUTESTRINGCONVERTERINTEGER_H
#define GRIDATTRIBUTESTRINGCONVERTERINTEGER_H

#include "../base/gridattributestringconverter.h"

class GridAttributeStringConverterInteger : public GridAttributeStringConverter
{
public:
	GridAttributeStringConverterInteger();

	QString convert(const QVariant& value) override;
};

#endif // GRIDATTRIBUTESTRINGCONVERTERINTEGER_H
