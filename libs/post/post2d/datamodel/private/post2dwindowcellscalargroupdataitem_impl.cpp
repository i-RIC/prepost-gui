#include "post2dwindowcellscalargroupdataitem_impl.h"

#include <guibase/widget/opacitycontainerwidget.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidget.h>

#include <vtkActor.h>
#include <vtkActor2D.h>

Post2dWindowCellScalarGroupDataItem::Impl::Impl(Post2dWindowCellScalarGroupDataItem* item) :
	m_target {},
	m_actor {vtkActor::New()},
	m_legendActor {vtkActor2D::New()},
	m_colorMapToolBarWidget {new ColorMapSettingToolBarWidget(item->mainWindow())},
	m_opacityToolBarWidget {new OpacityContainerWidget(item->mainWindow())}
{}

Post2dWindowCellScalarGroupDataItem::Impl::~Impl()
{
	m_actor->Delete();
	m_legendActor->Delete();
}
