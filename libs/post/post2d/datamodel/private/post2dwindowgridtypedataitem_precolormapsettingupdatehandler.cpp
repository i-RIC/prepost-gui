#include "post2dwindowgridtypedataitem_precolormapsettingupdatehandler.h"

#include <guicore/image/imagesettingcontainer.h>
#include <guicore/scalarstocolors/colormaplegendsettingcontaineri.h>
#include <guicore/scalarstocolors/delegatedcolormapsettingcontainer.h>

Post2dWindowGridTypeDataItem::PreColorMapSettingUpdateHandler::PreColorMapSettingUpdateHandler(DelegatedColorMapSettingContainer* setting, VTKGraphicsView* view, QObject *parent) :
	QObject {parent},
	m_setting {setting},
	m_view {view}
{}

void Post2dWindowGridTypeDataItem::PreColorMapSettingUpdateHandler::handle()
{
	if (! m_setting->usePreSetting) {return;}

	m_setting->preSetting->imgSetting()->apply(m_view);
}
