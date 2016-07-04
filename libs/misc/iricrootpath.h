#ifndef IRICROOTPATH_H
#define IRICROOTPATH_H

#include "misc_global.h"

class QString;

class MISCDLL_EXPORT iRICRootPath
{
public:
	static QString get();
	static void set(const QString& val);

private:
	iRICRootPath();
	~iRICRootPath();

	static QString m_value;
};

#endif // IRICROOTPATH_H
