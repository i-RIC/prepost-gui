#ifndef POST3DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_SETTINGEDITWIDGET_H
#define POST3DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_SETTINGEDITWIDGET_H

#include "../post3dwindowparticlesbasevectorgroupdataitem.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class Post3dWindowParticlesBaseVectorGroupDataItem_SettingEditWidget;
}

class GeneralFilteringSettingEditWidget;

class Post3dWindowParticlesBaseVectorGroupDataItem::SettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	SettingEditWidget(Post3dWindowParticlesBaseVectorGroupDataItem* item, QWidget *parent);
	~SettingEditWidget();

	QUndoCommand* createModifyCommand(bool apply) override;

private:
	GeneralFilteringSettingEditWidget* m_filteringEditWidget;

	Post3dWindowParticlesBaseVectorGroupDataItem* m_item;
	Ui::Post3dWindowParticlesBaseVectorGroupDataItem_SettingEditWidget *ui;
};

#endif // POST3DWINDOWPARTICLESBASEVECTORGROUPDATAITEM_SETTINGEDITWIDGET_H
