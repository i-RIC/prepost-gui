#ifndef POSTTITLEDATAITEM_EDITWIDGET_H
#define POSTTITLEDATAITEM_EDITWIDGET_H

#include "../posttitledataitem.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class PostTitleDataItem_EditWidget;
}

class PostTitleDataItem::EditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	EditWidget(PostTitleDataItem* item, QWidget *parent);
	~EditWidget();

	PostTitleSetting setting() const;
	void setSetting(const PostTitleSetting& setting);

	QUndoCommand* createModifyCommand(bool apply) override;

private:
	PostTitleDataItem* m_item;

	Ui::PostTitleDataItem_EditWidget *ui;
};

#endif // POSTTITLEDATAITEM_EDITWIDGET_H
