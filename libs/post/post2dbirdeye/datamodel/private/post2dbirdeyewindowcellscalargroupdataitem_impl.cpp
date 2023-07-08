#include "post2dbirdeyewindowcellscalargroupdataitem_impl.h"

#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidget.h>
#include <guicore/solverdef/solverdefinitiongridoutput.h>
#include <guibase/widget/opacitycontainerwidget.h>

Post2dBirdEyeWindowCellScalarGroupDataItem::Impl::Impl(const std::string& elevationTarget, Post2dBirdEyeWindowCellScalarGroupDataItem* item) :
	m_elevationTarget {elevationTarget},
	m_setting {},
	m_actor {vtkActor::New()},
	m_legendActor {vtkActor2D::New()},
	m_colorMapToolBarWidget {new ColorMapSettingToolBarWidget(item->mainWindow())},
	m_opacityToolBarWidget {new OpacityContainerWidget(item->mainWindow())}
{}

Post2dBirdEyeWindowCellScalarGroupDataItem::Impl::~Impl()
{
	for (const auto& pair : m_colorMapSettings) {
		delete pair.second;
	}

	m_actor->Delete();
	m_legendActor->Delete();
}

void Post2dBirdEyeWindowCellScalarGroupDataItem::Impl::createOrUpdateColorMapsSetting(SolverDefinitionGridOutput* output, const ValueRangeContainer& range)
{
	ColorMapSettingContainerI* setting = nullptr;
	auto it = m_colorMapSettings.find(output->name());
	if (it == m_colorMapSettings.end()) {
		setting = output->createColorMapSettingContainer();
		setting->valueCaption = output->caption();
		setting->legendSetting()->setTitle(output->caption());
		m_colorMapSettings.insert({output->name(), setting});
	} else {
		setting = it->second;
	}
	setting->setAutoValueRange(range);
}
