#ifndef POST2DWINDOWNODEVECTORARROWGROUPSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_H
#define POST2DWINDOWNODEVECTORARROWGROUPSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_H

#include "../post2dwindownodevectorarrowgroupstructureddataitem.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class Post2dWindowNodeVectorArrowGroupStructuredDataItem_SettingEditWidget;
}

class Post2dWindowNodeVectorArrowGroupStructuredDataItem::SettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	SettingEditWidget(Post2dWindowNodeVectorArrowGroupStructuredDataItem* item, QWidget *parent);
	~SettingEditWidget();

	QUndoCommand* createModifyCommand(bool apply);

private:
	Post2dWindowNodeVectorArrowGroupStructuredDataItem* m_item;

	class AdditionalWidgets;
	AdditionalWidgets* m_additionalWidgets;

	Ui::Post2dWindowNodeVectorArrowGroupStructuredDataItem_SettingEditWidget *ui;
};

#endif // POST2DWINDOWNODEVECTORARROWGROUPSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_H
