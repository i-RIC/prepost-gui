#include "post3dwindowcontourgroupdataitem_impl.h"

#include <guicore/scalarstocolors/colormapsettingtoolbarwidget.h>
#include <guibase/widget/opacitycontainerwidget.h>

#include <vtkActor2D.h>

Post3dWindowContourGroupDataItem::Impl::Impl(Post3dWindowContourGroupDataItem* item) :
	m_legendActor {vtkActor2D::New()},
	m_colorMapToolBarWidget {new ColorMapSettingToolBarWidget(item->mainWindow())},
	m_opacityToolBarWidget {new OpacityContainerWidget(item->mainWindow())}
{}

Post3dWindowContourGroupDataItem::Impl::~Impl()
{
	m_legendActor->Delete();
}
