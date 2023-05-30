#ifndef POST2DWINDOWPARTICLESBASESCALARGROUPDATAITEM_SETTINGEDITWIDGET_H
#define POST2DWINDOWPARTICLESBASESCALARGROUPDATAITEM_SETTINGEDITWIDGET_H

#include "../post2dwindowparticlesbasescalargroupdataitem.h"

#include <misc/modifycommandwidget.h>

namespace Ui {
class Post2dWindowParticlesBaseScalarGroupDataItem_SettingEditWidget;
}

class Post2dWindowParticlesBaseScalarGroupDataItem::SettingEditWidget : public ModifyCommandWidget
{
	Q_OBJECT

public:
	SettingEditWidget(Post2dWindowParticlesBaseScalarGroupDataItem* item, QWidget *parent);
	~SettingEditWidget();

	QUndoCommand* createModifyCommand(bool apply) override;

private:
	Post2dWindowParticlesBaseScalarGroupDataItem* m_item;
	Ui::Post2dWindowParticlesBaseScalarGroupDataItem_SettingEditWidget *ui;
};

#endif // POST2DWINDOWPARTICLESBASESCALARGROUPDATAITEM_SETTINGEDITWIDGET_H
