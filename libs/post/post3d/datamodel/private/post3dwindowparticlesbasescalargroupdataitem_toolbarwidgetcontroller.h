#ifndef POST3DWINDOWPARTICLESBASESCALARGROUPDATAITEM_TOOLBARWIDGETCONTROLLER_H
#define POST3DWINDOWPARTICLESBASESCALARGROUPDATAITEM_TOOLBARWIDGETCONTROLLER_H

#include "../post3dwindowparticlesbasescalargroupdataitem.h"

#include <guicore/scalarstocolors/colormapsettingtoolbarwidgetcontroller.h>

class Post3dWindowParticlesBaseScalarGroupDataItem::ToolBarWidgetController : public ColorMapSettingToolBarWidgetController
{
public:
	ToolBarWidgetController(ColorMapSettingToolBarWidget* widget, Post3dWindowParticlesBaseScalarGroupDataItem* item);

private slots:
	void handleWidgetUpdate() override;

private:
	Post3dWindowParticlesBaseScalarGroupDataItem* m_item;
};

#endif // POST3DWINDOWPARTICLESBASESCALARGROUPDATAITEM_TOOLBARWIDGETCONTROLLER_H
