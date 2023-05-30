#ifndef POST2DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_SETTINGEDITWIDGET_H
#define POST2DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_SETTINGEDITWIDGET_H

#include "../post2dwindowparticlesbasevectorgroupdataitem.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class Post2dWindowParticlesBaseVectorGroupDataItem_SettingEditWidget;
}

class GeneralFilteringSettingEditWidget;

class Post2dWindowParticlesBaseVectorGroupDataItem::SettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	SettingEditWidget(Post2dWindowParticlesBaseVectorGroupDataItem* itemm, QWidget *parent);
	~SettingEditWidget();

	QUndoCommand* createModifyCommand(bool apply) override;

private:
	GeneralFilteringSettingEditWidget* m_filteringEditWidget;

	Post2dWindowParticlesBaseVectorGroupDataItem* m_item;
	Ui::Post2dWindowParticlesBaseVectorGroupDataItem_SettingEditWidget *ui;
};

#endif // POST2DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_SETTINGEDITWIDGET_H
