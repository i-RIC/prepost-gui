#include "../../gridbirdeyewindow.h"
#include "gridbirdeyewindowcellscalargroupdataitem_impl.h"

#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidget.h>
#include <guibase/widget/opacitycontainerwidget.h>

GridBirdEyeWindowCellScalarGroupDataItem::Impl::Impl(const std::string& elevationTarget, GridBirdEyeWindowCellScalarGroupDataItem* item) :
	m_elevationTarget {elevationTarget},
	m_setting {},
	m_actor {vtkActor::New()},
	m_legendActor {vtkActor2D::New()},
	m_colorMapSettings {},
	m_colorMapToolBarWidget {new ColorMapSettingToolBarWidget(item->mainWindow())},
	m_opacityToolBarWidget {new OpacityContainerWidget(item->mainWindow())}
{}

GridBirdEyeWindowCellScalarGroupDataItem::Impl::~Impl()
{
	for (const auto& pair : m_colorMapSettings) {
		delete pair.second;
	}

	m_actor->Delete();
	m_legendActor->Delete();
}
