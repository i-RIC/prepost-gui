#ifndef GRIDATTRIBUTESTRINGCONVERTERREAL_H
#define GRIDATTRIBUTESTRINGCONVERTERREAL_H

#include "../base/gridattributestringconverter.h"

class GridAttributeStringConverterReal : public GridAttributeStringConverter
{
public:
	GridAttributeStringConverterReal();

	QString convert(const QVariant& value) override;
};

#endif // GRIDATTRIBUTESTRINGCONVERTERREAL_H
