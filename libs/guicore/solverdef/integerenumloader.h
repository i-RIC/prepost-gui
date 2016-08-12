#ifndef INTEGERENUMLOADER_H
#define INTEGERENUMLOADER_H

#include "enumloadert.h"

class IntegerEnumLoader : public EnumLoaderT<int>
{
private:
	int fromStr(const QString& str) const override;
};

#endif // INTEGERENUMLOADER_H
