#ifndef QSCREENUTIL_H
#define QSCREENUTIL_H

#include "misc_global.h"

class MISCDLL_EXPORT QScreenUtil
{
public:
	static QSize standardSizeWithinScreen();
	static QSize sizeWithinScreen(int w, int h);

private:
	QScreenUtil();
};

#endif // QSCREENUTIL_H
