#ifndef POST2DWINDOWPARTICLESBASESCALARGROUPDATAITEM_TOOLBARWIDGETCONTROLLER_H
#define POST2DWINDOWPARTICLESBASESCALARGROUPDATAITEM_TOOLBARWIDGETCONTROLLER_H

#include "../post2dwindowparticlesbasescalargroupdataitem.h"

#include <guicore/scalarstocolors/colormapsettingtoolbarwidgetcontroller.h>

class Post2dWindowParticlesBaseScalarGroupDataItem::ToolBarWidgetController : public ColorMapSettingToolBarWidgetController
{
public:
	ToolBarWidgetController(ColorMapSettingToolBarWidgetI* widget, Post2dWindowParticlesBaseScalarGroupDataItem* item);

private slots:
	void handleWidgetUpdate() override;

private:
	Post2dWindowParticlesBaseScalarGroupDataItem* m_item;
};

#endif // POST2DWINDOWPARTICLESBASESCALARGROUPDATAITEM_TOOLBARWIDGETCONTROLLER_H
