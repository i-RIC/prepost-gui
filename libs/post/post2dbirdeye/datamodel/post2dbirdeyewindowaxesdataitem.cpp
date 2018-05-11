#include "post2dbirdeyewindowaxesdataitem.h"

#include <guibase/graphicsmisc.h>
#include <guicore/datamodel/vtkgraphicsview.h>

#include <QStandardItem>

#include <vtkActorCollection.h>
#include <vtkCaptionActor2D.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>

Post2dBirdEyeWindowAxesDataItem::Post2dBirdEyeWindowAxesDataItem(GraphicsWindowDataItem* parent) :
	Post2dBirdEyeWindowDataItem {tr("Axes"), QIcon(":/libs/guibase/images/iconPaper.png"), parent}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	setupActors();
}

Post2dBirdEyeWindowAxesDataItem::~Post2dBirdEyeWindowAxesDataItem()
{
	m_widget->SetInteractor(0);
}

void Post2dBirdEyeWindowAxesDataItem::setupActors()
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

void Post2dBirdEyeWindowAxesDataItem::updateVisibility(bool visible)
{
	bool v = (m_standardItem->checkState() == Qt::Checked) && visible;
	m_widget->SetEnabled(v);
}

void Post2dBirdEyeWindowAxesDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	m_widget->SetInteractive(1);
}

void Post2dBirdEyeWindowAxesDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	m_widget->SetInteractive(0);
}

void Post2dBirdEyeWindowAxesDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseMoveEvent(event);
}

void Post2dBirdEyeWindowAxesDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMousePressEvent(event);
}

void Post2dBirdEyeWindowAxesDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseReleaseEvent(event);
}

