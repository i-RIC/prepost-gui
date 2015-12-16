#include "qttool.h"

#include <QStandardItem>

void QtTool::checkItem(QStandardItem* item, bool checked)
{
	if (checked) {
		item->setCheckState(Qt::Checked);
	} else {
		item->setCheckState(Qt::Unchecked);
	}
}
