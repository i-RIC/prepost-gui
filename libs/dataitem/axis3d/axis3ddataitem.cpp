#include "axis3ddataitem.h"
#include "private/axis3ddataitem_impl.h"

#include <guibase/graphicsmisc.h>
#include <guicore/datamodel/vtkgraphicsview.h>

#include <QStandardItem>

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

Axis3dDataItem::Impl::Impl(Axis3dDataItem* parent) :
	m_parent {parent}
{
	m_actor = vtkSmartPointer<vtkAxesActor>::New();
	m_widget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
}

Axis3dDataItem::Impl::~Impl()
{
	m_widget->SetInteractor(0);
}

void Axis3dDataItem::Impl::setupActors()
{
	iRIC::setupAxesActor(m_actor);
	m_actor->SetTotalLength(1, 1, 1);

	m_widget->SetOutlineColor(.5, .5, .5);
	m_widget->SetOrientationMarker(m_actor);
	vtkRenderWindowInteractor* iren = m_parent->renderer()->GetRenderWindow()->GetInteractor();
	m_widget->SetInteractor(iren);
	m_widget->SetViewport(0.0, 0.0, 0.2, 0.2);
	m_widget->SetEnabled(1);
	m_widget->InteractiveOff();
}

// public interfaces

Axis3dDataItem::Axis3dDataItem(GraphicsWindowDataItem* parent) :
	GraphicsWindowDataItem {tr("Axes"), QIcon(":/libs/guibase/images/iconPaper.png"), parent},
	impl {new Impl(this)}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	impl->setupActors();
}

Axis3dDataItem::~Axis3dDataItem()
{
	delete impl;
}

void Axis3dDataItem::updateVisibility(bool visible)
{
	bool v = (m_standardItem->checkState() == Qt::Checked) && visible;
	impl->m_widget->SetEnabled(v);
}

void Axis3dDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	impl->m_widget->SetInteractive(1);
}

void Axis3dDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	impl->m_widget->SetInteractive(0);
}

void Axis3dDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseMoveEvent(event);
}

void Axis3dDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMousePressEvent(event);
}

void Axis3dDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseReleaseEvent(event);
}

void Axis3dDataItem::doLoadFromProjectMainFile(const QDomNode&)
{}

void Axis3dDataItem::doSaveToProjectMainFile(QXmlStreamWriter&)
{}
