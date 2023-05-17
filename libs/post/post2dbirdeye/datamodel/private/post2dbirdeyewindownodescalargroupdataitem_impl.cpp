#include "post2dbirdeyewindownodescalargroupdataitem_impl.h"

#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidget.h>
#include <guibase/widget/opacitycontainerwidget.h>

Post2dBirdEyeWindowNodeScalarGroupDataItem::Impl::Impl(const std::string& elevationTarget, Post2dBirdEyeWindowNodeScalarGroupDataItem* item) :
	m_elevationTarget {elevationTarget},
	m_setting {},
	m_actor {vtkActor::New()},
	m_legendActor {vtkActor2D::New()},
	m_colorMapSettings {},
	m_colorMapToolBarWidget {new ColorMapSettingToolBarWidget(item->mainWindow())},
	m_opacityToolBarWidget {new OpacityContainerWidget(item->mainWindow())}
{}

Post2dBirdEyeWindowNodeScalarGroupDataItem::Impl::~Impl()
{
	for (const auto& pair : m_colorMapSettings) {
		delete pair.second;
	}

	m_actor->Delete();
	m_legendActor->Delete();
}
