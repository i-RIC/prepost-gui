#ifndef GRIDATTRIBUTESTRINGCONVERTERSTRING_H
#define GRIDATTRIBUTESTRINGCONVERTERSTRING_H

#include "../base/gridattributestringconverter.h"

class GridAttributeStringConverterString : public GridAttributeStringConverter
{
public:
	GridAttributeStringConverterString();

	QString convert(const QVariant& value) override;
};

#endif // GRIDATTRIBUTESTRINGCONVERTERSTRING_H
