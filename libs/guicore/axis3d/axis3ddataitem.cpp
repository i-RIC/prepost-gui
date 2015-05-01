#include "../datamodel/vtkgraphicsview.h"
#include "axis3ddataitem.h"

#include <guibase/graphicsmisc.h>

#include <vtkActorCollection.h>
#include <vtkCaptionActor2D.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

Axis3dDataItem::Axis3dDataItem(GraphicsWindowDataItem* parent)
	: GraphicsWindowDataItem(tr("Axes"), QIcon(":/libs/guibase/images/iconPaper.png"), parent)
{
	m_isDeletable = false;

	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();

	setupActors();
}

Axis3dDataItem::~Axis3dDataItem()
{
	m_widget->SetInteractor(0);
}

void Axis3dDataItem::setupActors()
{
	m_actor = vtkSmartPointer<vtkAxesActor>::New();
	iRIC::setupAxesActor(m_actor);
	m_actor->SetTotalLength(1, 1, 1);

	m_widget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
	m_widget->SetOutlineColor(.5, .5, .5);
	m_widget->SetOrientationMarker(m_actor);
	vtkRenderWindowInteractor* iren = renderer()->GetRenderWindow()->GetInteractor();
	m_widget->SetInteractor(iren);
	m_widget->SetViewport(0.0, 0.0, 0.2, 0.2);
	m_widget->SetEnabled(1);
	m_widget->InteractiveOff();
}

void Axis3dDataItem::updateVisibility(bool visible)
{
	bool v = (m_standardItem->checkState() == Qt::Checked) && visible;
	m_widget->SetEnabled(v);
}

void Axis3dDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	m_widget->SetInteractive(1);
}

void Axis3dDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	m_widget->SetInteractive(0);
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

