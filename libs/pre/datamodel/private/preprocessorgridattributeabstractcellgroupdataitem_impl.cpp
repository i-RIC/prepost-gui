#include "preprocessorgridattributeabstractcellgroupdataitem_impl.h"

#include <guibase/widget/opacitycontainerwidget.h>
#include <misc/qwidgetcontainer.h>

#include <vtkActor.h>

#include <QAction>

PreProcessorGridAttributeAbstractCellGroupDataItem::Impl::Impl(QWidget* mainWindow, PreProcessorGridAttributeAbstractCellGroupDataItem* item) :
	m_setting {},
	m_target {},
	m_actor {vtkActor::New()},
	m_showAttributeBrowserAction {new QAction(PreProcessorGridAttributeAbstractCellGroupDataItem::tr("Show Attribute Browser"), item)},
	m_attributeBrowserFixed {false},
	m_opacityWidget {new OpacityContainerWidget(mainWindow)},
	m_colorMapWidgetContainer {new QWidgetContainer(mainWindow)}
{
	m_actor->GetProperty()->SetLighting(false);
}

PreProcessorGridAttributeAbstractCellGroupDataItem::Impl::~Impl()
{
	m_actor->Delete();
}
