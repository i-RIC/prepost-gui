#ifndef POST2DWINDOWPARTICLESBASESCALARGROUPDATAITEM_TOOLBARWIDGET_H
#define POST2DWINDOWPARTICLESBASESCALARGROUPDATAITEM_TOOLBARWIDGET_H

#include "../post2dwindowparticlesbasescalargroupdataitem.h"

#include <QWidget>

namespace Ui {
class Post2dWindowParticlesBaseScalarGroupDataItem_ToolBarWidget;
}

class Post2dWindowParticlesBaseScalarGroupDataItem::ToolBarWidget : public QWidget
{
	Q_OBJECT

public:
	ToolBarWidget(Post2dWindowParticlesBaseScalarGroupDataItem* item, QWidget *parent = nullptr);
	~ToolBarWidget();

public slots:
	void applySetting();

private slots:
	void updateActorSetting();

private:
	ParticleDataSetting modifiedSetting() const;

	Post2dWindowParticlesBaseScalarGroupDataItem* m_item;
	Ui::Post2dWindowParticlesBaseScalarGroupDataItem_ToolBarWidget *ui;
};

#endif // POST2DWINDOWPARTICLESBASESCALARGROUPDATAITEM_TOOLBARWIDGET_H
