#ifndef MODIFYCOMMANDWIDGET_H
#define MODIFYCOMMANDWIDGET_H

#include "misc_global.h"

#include <QWidget>

class MISCDLL_EXPORT ModifyCommandWidget : public QWidget
{
public:
	ModifyCommandWidget(QWidget* widget);
	virtual ~ModifyCommandWidget();

	virtual QUndoCommand* createModifyCommand(bool apply) = 0;
};

#endif // MODIFYCOMMANDWIDGET_H
