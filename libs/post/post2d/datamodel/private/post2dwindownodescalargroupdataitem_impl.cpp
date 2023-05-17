#include "post2dwindownodescalargroupdataitem_impl.h"
#include "post2dwindownodescalargroupdataitem_shapeexporter.h"

#include <guicore/scalarstocolors/colormapsettingtoolbarwidget.h>
#include <guibase/widget/opacitycontainerwidget.h>

#include <vtkActor.h>
#include <vtkActor2D.h>

Post2dWindowNodeScalarGroupDataItem::Impl::Impl(Post2dWindowNodeScalarGroupDataItem* item) :
	m_target {},
	m_solutionPosition {iRICLib::H5CgnsZone::SolutionPosition::Node},
	m_setting {},
	m_actor {vtkActor::New()},
	m_legendActor {vtkActor2D::New()},
	m_colorMapToolBarWidget {new ColorMapSettingToolBarWidget(item->mainWindow())},
	m_opacityToolBarWidget {new OpacityContainerWidget(item->mainWindow())},
	m_shapeExporter {new ShapeExporter(item)}
{}

Post2dWindowNodeScalarGroupDataItem::Impl::~Impl()
{
	m_actor->Delete();
	m_legendActor->Delete();

	delete m_shapeExporter;
}
