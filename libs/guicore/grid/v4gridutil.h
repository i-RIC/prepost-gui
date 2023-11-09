#ifndef V4GRIDUTIL_H
#define V4GRIDUTIL_H

#include "../guicore_global.h"

class GUICOREDLL_EXPORT v4GridUtil
{
public:
	const static int DEFAULT_MAX_DRAW_CELL_COUNT;
	const static int DEFAULT_MAX_DRAW_INDEX_COUNT;
	const static char LABEL_NAME[];

	static void getCullSetting(bool* enable, int* cellLimit, int* indexLimit);

private:
	v4GridUtil();
};

#endif // V4GRIDUTIL_H
