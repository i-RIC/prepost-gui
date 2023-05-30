#ifndef POST3DWINDOWPARTICLESBASESCALARGROUPDATAITEM_SETTINGEDITWIDGET_H
#define POST3DWINDOWPARTICLESBASESCALARGROUPDATAITEM_SETTINGEDITWIDGET_H

#include "../post3dwindowparticlesbasescalargroupdataitem.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class Post3dWindowParticlesBaseScalarGroupDataItem_SettingEditWidget;
}

class Post3dWindowParticlesBaseScalarGroupDataItem::SettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	SettingEditWidget(Post3dWindowParticlesBaseScalarGroupDataItem* item, QWidget *parent);
	~SettingEditWidget();

	QUndoCommand* createModifyCommand(bool apply) override;

private:
	Post3dWindowParticlesBaseScalarGroupDataItem* m_item;
	Ui::Post3dWindowParticlesBaseScalarGroupDataItem_SettingEditWidget *ui;
};

#endif // POST3DWINDOWPARTICLESBASESCALARGROUPDATAITEM_SETTINGEDITWIDGET_H
