#include "gridcreatingconditioncenterandwidth_impl.h"

#include <guicore/pre/base/preprocessorgraphicsviewi.h>
#include <misc/mathsupport.h>

#include <QAction>
#include <QIcon>

#include <vtkCardinalSpline.h>

GridCreatingConditionCenterAndWidth::Impl::Impl(GridCreatingConditionCenterAndWidth* parent) :
	m_selectedVertexId {0},
	m_selectedEdgeId {0},
	m_oldIMax {0},
	m_oldJMax {0},
	m_oldWidth {0},
	m_polyLineController {},
	m_upstreamActor {},
	m_downstreamActor {},
	m_previewActor {vtkActor::New()},
	m_previewMapper {vtkDataSetMapper::New()},
	m_previewGrid {nullptr},
	m_xSpline {vtkCardinalSpline::New()},
	m_ySpline {vtkCardinalSpline::New()},
	m_zSpline {vtkCardinalSpline::New()},
	m_spline {vtkParametricSpline::New()},
	m_splinePoints {vtkPoints::New()},
	m_mouseEventMode {MouseEventMode::BeforeDefining},
	m_isAccepted {false},
	m_isGridCreated {false},
	m_dragStartPoint {},
	m_previousPoint {},
	m_addPixmap {":/libs/guibase/images/cursorAdd.png"},
	m_removePixmap {":/libs/guibase/images/cursorRemove.png"},
	m_addCursor {m_addPixmap, 0, 0},
	m_removeCursor {m_removePixmap, 0, 0},
	m_rightClickingMenu {new QMenu {}},
	m_addVertexAction {new QAction(QIcon(":/libs/guibase/images/iconAddPolygonVertex.svg"), GridCreatingConditionCenterAndWidth::tr("&Add Vertex"), parent)},
	m_removeVertexAction {new QAction(QIcon(":/libs/guibase/images/iconRemovePolygonVertex.svg"), GridCreatingConditionCenterAndWidth::tr("&Remove Vertex"), parent)},
	m_coordEditAction {new QAction(GridCreatingConditionCenterAndWidth::tr("Edit C&oordinates..."), parent)},
	m_reverseCenterLineAction {new QAction(GridCreatingConditionCenterAndWidth::tr("R&everse Center Line Direction"), parent)},
	m_importCenterLineAction {new QAction(QIcon(":/libs/guibase/images/iconImport.svg"), GridCreatingConditionCenterAndWidth::tr("&Import Center Line..."), parent)},
	m_exportCenterLineAction {new QAction(QIcon(":/libs/guibase/images/iconExport.svg"), GridCreatingConditionCenterAndWidth::tr("&Export Center Line..."), parent)},
	m_parent {parent}
{
	// setup VTK objects
	auto prop = m_previewActor->GetProperty();
	prop->SetLighting(false);
	prop->SetColor(0, 0, 0);
	prop->SetRepresentationToWireframe();

	m_previewActor->SetMapper(m_previewMapper);

	// spline related objects
	m_spline->SetXSpline(m_xSpline);
	m_spline->SetYSpline(m_ySpline);
	m_spline->SetZSpline(m_zSpline);
	m_splinePoints->SetDataTypeToDouble();

	// setup actions
	m_addVertexAction->setCheckable(true);
	connect(m_addVertexAction, &QAction::triggered, parent, &GridCreatingConditionCenterAndWidth::addVertexMode);

	m_removeVertexAction->setCheckable(true);
	connect(m_removeVertexAction, &QAction::triggered, parent, &GridCreatingConditionCenterAndWidth::removeVertexMode);

	connect(m_coordEditAction, &QAction::triggered, parent, &GridCreatingConditionCenterAndWidth::editCoordinates);

	connect(m_reverseCenterLineAction, &QAction::triggered, parent, &GridCreatingConditionCenterAndWidth::reverseCenterLineDirection);

	connect(m_importCenterLineAction, &QAction::triggered, parent, &GridCreatingConditionCenterAndWidth::importCenterLine);

	connect(m_exportCenterLineAction, &QAction::triggered, parent, &GridCreatingConditionCenterAndWidth::exportCenterLine);
}

GridCreatingConditionCenterAndWidth::Impl::~Impl()
{
	m_xSpline->Delete();
	m_ySpline->Delete();
	m_zSpline->Delete();
	m_spline->Delete();

	m_previewActor->Delete();
	m_previewMapper->Delete();
}

void GridCreatingConditionCenterAndWidth::Impl::updateMouseEventMode(const QPoint& pos, PreProcessorGraphicsViewI* view)
{
	auto worldPos = view->viewportToWorld(pos);
	double radius = view->stdRadius(iRIC::nearRadius());

	switch (m_mouseEventMode) {
	case MouseEventMode::AddVertexNotPossible:
	case MouseEventMode::AddVertexPrepare:
		if (m_polyLineController.isEdgeSelectable(worldPos, radius, &m_selectedEdgeId)) {
			m_mouseEventMode = MouseEventMode::AddVertexPrepare;
		} else {
			m_mouseEventMode = MouseEventMode::AddVertexNotPossible;
		}
		break;
	case MouseEventMode::RemoveVertexNotPossible:
	case MouseEventMode::RemoveVertexPrepare:
		if (m_polyLineController.isVertexSelectable(worldPos, radius, &m_selectedVertexId)) {
			m_mouseEventMode = MouseEventMode::RemoveVertexPrepare;
		} else {
			m_mouseEventMode = MouseEventMode::RemoveVertexNotPossible;
		}
		break;
	case MouseEventMode::Normal:
	case MouseEventMode::TranslatePrepare:
	case MouseEventMode::MoveVertexPrepare:
	case MouseEventMode::Translate:
	case MouseEventMode::MoveVertex:
	case MouseEventMode::AddVertex:
		if (m_polyLineController.isVertexSelectable(worldPos, radius, &m_selectedVertexId)) {
			m_mouseEventMode = MouseEventMode::MoveVertexPrepare;
		} else if (m_polyLineController.isEdgeSelectable(worldPos, radius, &m_selectedEdgeId)) {
			m_mouseEventMode = MouseEventMode::TranslatePrepare;
		} else {
			m_mouseEventMode = MouseEventMode::Normal;
		}
		break;
	case MouseEventMode::BeforeDefining:
	case MouseEventMode::Defining:
		// do nothing
		break;
	case MouseEventMode::EditVerticesDialog:
		break;
	}
}

void GridCreatingConditionCenterAndWidth::Impl::updateMouseCursor(PreProcessorGraphicsViewI* v)
{

	switch (m_mouseEventMode) {
	case MouseEventMode::Normal:
	case MouseEventMode::AddVertexNotPossible:
	case MouseEventMode::RemoveVertexNotPossible:
	case MouseEventMode::EditVerticesDialog:
		v->setCursor(Qt::ArrowCursor);
		break;
	case MouseEventMode::BeforeDefining:
	case MouseEventMode::Defining:
		v->setCursor(Qt::CrossCursor);
		break;
	case MouseEventMode::TranslatePrepare:
	case MouseEventMode::MoveVertexPrepare:
		v->setCursor(Qt::OpenHandCursor);
		break;
	case MouseEventMode::AddVertexPrepare:
		v->setCursor(m_addCursor);
		break;
	case MouseEventMode::RemoveVertexPrepare:
		v->setCursor(m_removeCursor);
		break;
	case MouseEventMode::Translate:
	case MouseEventMode::MoveVertex:
	case MouseEventMode::AddVertex:
		v->setCursor(Qt::ClosedHandCursor);
		break;
	}
}

void GridCreatingConditionCenterAndWidth::Impl::updateActionStatus()
{
	switch (m_mouseEventMode) {
	case MouseEventMode::BeforeDefining:
	case MouseEventMode::Defining:
	case MouseEventMode::Translate:
	case MouseEventMode::MoveVertex:
		m_addVertexAction->setDisabled(true);
		m_addVertexAction->setChecked(false);
		m_removeVertexAction->setDisabled(true);
		m_removeVertexAction->setChecked(false);
		m_coordEditAction->setEnabled(false);
		break;

	case MouseEventMode::Normal:
	case MouseEventMode::TranslatePrepare:
	case MouseEventMode::MoveVertexPrepare:
		m_addVertexAction->setEnabled(true);
		m_addVertexAction->setChecked(false);
		m_removeVertexAction->setEnabled(m_polyLineController.polyData()->GetPoints()->GetNumberOfPoints() > 2);
		m_removeVertexAction->setChecked(false);
		m_coordEditAction->setEnabled(true);
		break;

	case MouseEventMode::AddVertexPrepare:
	case MouseEventMode::AddVertexNotPossible:
	case MouseEventMode::AddVertex:
		m_addVertexAction->setEnabled(true);
		m_addVertexAction->setChecked(true);
		m_removeVertexAction->setDisabled(true);
		m_removeVertexAction->setChecked(false);
		m_coordEditAction->setEnabled(false);
		break;

	case MouseEventMode::RemoveVertexPrepare:
	case MouseEventMode::RemoveVertexNotPossible:
		m_addVertexAction->setDisabled(true);
		m_addVertexAction->setChecked(false);
		m_removeVertexAction->setEnabled(true);
		m_removeVertexAction->setChecked(true);
		m_coordEditAction->setEnabled(false);
		break;

	case MouseEventMode::EditVerticesDialog:
		break;
	}
}
