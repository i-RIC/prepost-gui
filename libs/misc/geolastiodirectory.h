#ifndef GEOLASTIODIRECTORY_H
#define GEOLASTIODIRECTORY_H

#include "misc_global.h"

class MISCDLL_EXPORT GeoLastIODirectory
{
public:
	static QString get();
	static void set(const QString& val);
	static void setFromFilename(const QString& filename);

private:
	GeoLastIODirectory();
};

#endif // GEOLASTIODIRECTORY_H
