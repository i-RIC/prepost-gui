#include "v4gridutil.h"

#include <QSettings>

const int v4GridUtil::DEFAULT_MAX_DRAW_CELL_COUNT = 40000;
const int v4GridUtil::DEFAULT_MAX_DRAW_INDEX_COUNT = 300;
const char v4GridUtil::LABEL_NAME[] = "_LABEL";

void v4GridUtil::getCullSetting(bool* enable, int* cellLimit, int* indexLimit)
{
	QSettings settings;

	*enable = settings.value("general/cullcellenable", true).toBool();
	*cellLimit = settings.value("general/cullcelllimit", DEFAULT_MAX_DRAW_CELL_COUNT).toInt();
	*indexLimit = settings.value("general/cullindexlimit", DEFAULT_MAX_DRAW_INDEX_COUNT).toInt();
}

v4GridUtil::v4GridUtil()
{}
