#ifndef GRIDATTRIBUTESTRINGCONVERTER_H
#define GRIDATTRIBUTESTRINGCONVERTER_H

#include "../../../guicore_global.h"

class QString;
class QVariant;

class GUICOREDLL_EXPORT GridAttributeStringConverter
{
public:
	GridAttributeStringConverter();
	virtual ~GridAttributeStringConverter();

	virtual QString convert(const QVariant& value) = 0;
};

#endif // GRIDATTRIBUTESTRINGCONVERTER_H
