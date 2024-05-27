#ifndef POSTTIMEDATAITEM_EDITWIDGET_H
#define POSTTIMEDATAITEM_EDITWIDGET_H

#include "../posttimedataitem.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class PostTimeDataItem_EditWidget;
}

class PostTimeDataItem::EditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	EditWidget(PostTimeDataItem* item, QWidget *parent);
	~EditWidget();

	Setting setting() const;
	void setSetting(const Setting& setting);

	void setActualTimeAvailable(bool available);

	QUndoCommand* createModifyCommand(bool apply) override;

private:
	PostTimeDataItem* m_item;

	Ui::PostTimeDataItem_EditWidget *ui;
};

#endif // POSTTIMEDATAITEM_EDITWIDGET_H
