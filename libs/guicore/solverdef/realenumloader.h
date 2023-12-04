#ifndef REALENUMLOADER_H
#define REALENUMLOADER_H

#include "../guicore_global.h"

#include "enumloadert.h"

class GUICOREDLL_EXPORT RealEnumLoader : public EnumLoaderT<double>
{
private:
	double fromStr(const QString& str) const override;
};

#endif // REALENUMLOADER_H
