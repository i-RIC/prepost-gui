#ifndef POST3DWINDOWPARTICLESBASESCALARGROUPDATAITEM_TOOLBARWIDGET_H
#define POST3DWINDOWPARTICLESBASESCALARGROUPDATAITEM_TOOLBARWIDGET_H

#include "../post3dwindowparticlesbasescalargroupdataitem.h"

#include <QWidget>

namespace Ui {
class Post3dWindowParticlesBaseScalarGroupDataItem_ToolBarWidget;
}

class Post3dWindowParticlesBaseScalarGroupDataItem::ToolBarWidget : public QWidget
{
	Q_OBJECT

public:
	ToolBarWidget(Post3dWindowParticlesBaseScalarGroupDataItem* item, QWidget *parent = nullptr);
	~ToolBarWidget();

private slots:
	void applySetting();
	void updateActorSetting();

private:
	ParticleDataSetting modifiedSetting() const;

	Post3dWindowParticlesBaseScalarGroupDataItem* m_item;
	Ui::Post3dWindowParticlesBaseScalarGroupDataItem_ToolBarWidget *ui;
};

#endif // POST3DWINDOWPARTICLESBASESCALARGROUPDATAITEM_TOOLBARWIDGET_H
