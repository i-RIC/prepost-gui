#include "../datamodel/vtkgraphicsview.h"
#include "axis2ddataitem.h"

#include <guibase/graphicsmisc.h>

#include <vtkActorCollection.h>
#include <vtkCaptionActor2D.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

Axis2dDataItem::Axis2dDataItem(GraphicsWindowDataItem* parent)
	: GraphicsWindowDataItem(tr("Axes"), QIcon(":/libs/guibase/images/iconPaper.png"), parent)
{
	m_isDeletable = false;

	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();

	setupActors();
}

Axis2dDataItem::~Axis2dDataItem()
{
	m_widget->SetInteractor(0);
}

void Axis2dDataItem::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{
	// @todo we have to enable/disable m_widget here.
}

void Axis2dDataItem::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{

}

void Axis2dDataItem::setupActors()
{
	m_actor = vtkSmartPointer<vtkAxesActor>::New();
	iRIC::setupAxesActor(m_actor);
	m_actor->SetTotalLength(1, 1, 0);

#ifdef _DEBUG
	m_actor->SetZAxisLabelText(".");
#else
	m_actor->SetZAxisLabelText("");
#endif

	m_widget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
	m_widget->SetOutlineColor(.5, .5, .5);
	m_widget->SetOrientationMarker(m_actor);
	vtkRenderWindowInteractor* iren = renderer()->GetRenderWindow()->GetInteractor();
	m_widget->SetInteractor(iren);
	m_widget->SetViewport(0.0, 0.0, 0.2, 0.2);
	m_widget->SetEnabled(1);
	m_widget->InteractiveOff();
}

void Axis2dDataItem::updateVisibility(bool visible)
{
	bool v = (m_standardItem->checkState() == Qt::Checked) && visible;
	m_widget->SetEnabled(v);
}

void Axis2dDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	m_widget->SetInteractive(1);
}

void Axis2dDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	m_widget->SetInteractive(0);
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

