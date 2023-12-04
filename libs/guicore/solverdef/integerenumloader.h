#ifndef INTEGERENUMLOADER_H
#define INTEGERENUMLOADER_H

#include "../guicore_global.h"

#include "enumloadert.h"

class GUICOREDLL_EXPORT IntegerEnumLoader : public EnumLoaderT<int>
{
private:
	int fromStr(const QString& str) const override;
};

#endif // INTEGERENUMLOADER_H
