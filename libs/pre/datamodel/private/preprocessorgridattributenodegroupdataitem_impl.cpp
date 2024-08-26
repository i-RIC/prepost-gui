#include "preprocessorgridattributenodegroupdataitem_impl.h"

#include <guibase/widget/opacitycontainerwidget.h>
#include <misc/qwidgetcontainer.h>

#include <vtkActor.h>
#include <vtkActor2D.h>

#include <QAction>

PreProcessorGridAttributeNodeGroupDataItem::Impl::Impl(QWidget* mainWindow, PreProcessorGridAttributeNodeGroupDataItem* item) :
	m_setting {},
	m_target {},
	m_actor {vtkActor::New()},
	m_stringActor {vtkActor2D::New()},
	m_showAttributeBrowserAction {new QAction(PreProcessorGridAttributeNodeGroupDataItem::tr("Show Attribute Browser"), item)},
	m_attributeBrowserFixed {false},
	m_opacityWidget {new OpacityContainerWidget(mainWindow)},
	m_colorMapWidgetContainer {new QWidgetContainer(mainWindow)}
{
	m_actor->GetProperty()->SetLighting(false);
}

PreProcessorGridAttributeNodeGroupDataItem::Impl::~Impl()
{
	m_actor->Delete();
	m_stringActor->Delete();
}
