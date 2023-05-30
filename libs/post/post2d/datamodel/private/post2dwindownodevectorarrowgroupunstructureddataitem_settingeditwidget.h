#ifndef POST2DWINDOWNODEVECTORARROWGROUPUNSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_H
#define POST2DWINDOWNODEVECTORARROWGROUPUNSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_H

#include "../post2dwindownodevectorarrowgroupunstructureddataitem.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class Post2dWindowNodeVectorArrowGroupUnstructuredDataItem_SettingEditWidget;
}

class Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::SettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	SettingEditWidget(Post2dWindowNodeVectorArrowGroupUnstructuredDataItem* item, QWidget *parent);
	~SettingEditWidget();

	QUndoCommand* createModifyCommand(bool apply) override;

private:
	Post2dWindowNodeVectorArrowGroupUnstructuredDataItem* m_item;

	class AdditionalWidgets;
	AdditionalWidgets* m_additionalWidgets;

	Ui::Post2dWindowNodeVectorArrowGroupUnstructuredDataItem_SettingEditWidget *ui;
};

#endif // POST2DWINDOWNODEVECTORARROWGROUPUNSTRUCTUREDDATAITEM_SETTINGEDITWIDGET_H
