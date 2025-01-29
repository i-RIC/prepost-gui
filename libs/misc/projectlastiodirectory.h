#ifndef PROJECTLASTIODIRECTORY_H
#define PROJECTLASTIODIRECTORY_H

#include "misc_global.h"

class MISCDLL_EXPORT ProjectLastIODirectory
{
public:
	static QString get();
	static void set(const QString& val);
	static void setFromFilename(const QString& filename);

private:
	ProjectLastIODirectory();
};

#endif // PROJECTLASTIODIRECTORY_H
