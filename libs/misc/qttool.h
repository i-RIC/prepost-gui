#ifndef QTTOOL_H
#define QTTOOL_H

#include "misc_global.h"

class QStandardItem;
class QNetworkAccessManager;

class MISCDLL_EXPORT QtTool
{
public:
	static void checkItem(QStandardItem* item, bool check);

	static void initNetworkAccessManager();
	static void destroyNetworkAccessManager();
	static QNetworkAccessManager* networkAccessManager();

private:
	QtTool();
};

#endif // QTTOOL_H
