#ifndef QTTOOL_H
#define QTTOOL_H

#include "misc_global.h"

class QStandardItem;

class MISCDLL_EXPORT QtTool
{
public:
	static void checkItem(QStandardItem* item, bool check);

private:
	QtTool();
};

#endif // QTTOOL_H
