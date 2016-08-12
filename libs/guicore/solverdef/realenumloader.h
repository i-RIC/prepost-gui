#ifndef REALENUMLOADER_H
#define REALENUMLOADER_H

#include "enumloadert.h"

class RealEnumLoader : public EnumLoaderT<double>
{
private:
	double fromStr(const QString& str) const override;
};

#endif // REALENUMLOADER_H
