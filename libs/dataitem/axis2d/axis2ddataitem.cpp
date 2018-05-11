#include "axis2ddataitem.h"
#include "private/axis2ddataitem_impl.h"

#include <guibase/graphicsmisc.h>
#include <guicore/datamodel/vtkgraphicsview.h>

#include <QIcon>
#include <QStandardItem>

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

Axis2dDataItem::Impl::Impl(Axis2dDataItem* parent) :
	m_parent {parent}
{
	m_actor = vtkSmartPointer<vtkAxesActor>::New();
	m_widget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
}

Axis2dDataItem::Impl::~Impl()
{
	m_widget->SetInteractor(0);
}

void Axis2dDataItem::Impl::setupActors()
{
	iRIC::setupAxesActor(m_actor);
	m_actor->SetTotalLength(1, 1, 0);

#ifdef _DEBUG
	m_actor->SetZAxisLabelText(".");
#else
	m_actor->SetZAxisLabelText("");
#endif

	m_widget->SetOutlineColor(.5, .5, .5);
	m_widget->SetOrientationMarker(m_actor);
	vtkRenderWindowInteractor* iren = m_parent->renderer()->GetRenderWindow()->GetInteractor();
	m_widget->SetInteractor(iren);
	m_widget->SetViewport(0.0, 0.0, 0.2, 0.2);
	m_widget->SetEnabled(1);
	m_widget->InteractiveOff();
}

// public interfaces

Axis2dDataItem::Axis2dDataItem(GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem {tr("Axes"), QIcon(":/libs/guibase/images/iconPaper.png"), parent},
	impl {new Impl(this)}
{
	setupStandardItem(Checked, NotReorderable, Deletable);

	impl->setupActors();
}

Axis2dDataItem::~Axis2dDataItem()
{
	delete impl;
}

void Axis2dDataItem::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{}

void Axis2dDataItem::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{}

void Axis2dDataItem::updateVisibility(bool visible)
{
	bool v = (m_standardItem->checkState() == Qt::Checked) && visible;
	impl->m_widget->SetEnabled(v);
}

void Axis2dDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	impl->m_widget->SetInteractive(1);
}

void Axis2dDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	impl->m_widget->SetInteractive(0);
}

void Axis2dDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseMoveEvent(event);
}

void Axis2dDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMousePressEvent(event);
}

void Axis2dDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseReleaseEvent(event);
}

