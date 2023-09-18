#include "geodatapointmap_pointsmanager.h"
#include "geodatapointmap_tinmanager_actions.h"
#include "geodatapointmap_tinmanager_addbreaklinecontroller.h"
#include "geodatapointmap_tinmanager_addbreaklinecontroller_addpointcommand.h"
#include "geodatapointmap_tinmanager_breakline.h"
#include "geodatapointmap_tinmanager_impl.h"

#include <guicore/pre/base/preprocessordatamodeli.h>
#include <guicore/pre/base/preprocessorgraphicsviewi.h>
#include <guicore/pre/base/preprocessorwindowi.h>
#include <guicore/datamodel/vtk2dgraphicsview.h>
#include <misc/informationdialog.h>
#include <misc/iricundostack.h>
#include <misc/keyboardsupport.h>
#include <misc/mathsupport.h>

namespace {

const vtkIdType NOT_SELECTED = -1;

} // namespace

GeoDataPointmap::TINManager::AddBreakLineController::AddBreakLineController(TINManager* manager) :
	m_addPixmap {":/libs/guibase/images/cursorAdd.png"},
	m_addCursor {m_addPixmap, 0, 0},
	m_breakLine {nullptr},
	m_manager {manager}
{}

void GeoDataPointmap::TINManager::AddBreakLineController::setBreakLine(BreakLine* line)
{
	m_breakLine = line;
}

void GeoDataPointmap::TINManager::AddBreakLineController::activate(VTK2DGraphicsView* /*v*/)
{
	auto w = m_manager->impl->m_parent->preProcessorWindow();
	InformationDialog::information(w, GeoDataPointmap::tr("Information"), GeoDataPointmap::tr("Break line can be defined by mouse-clicking on the points between which you want to add break line. Finish defining by double clicking, or pressing return key."), "geodatapointmapaddbreakline");

	m_currentVertex = NOT_SELECTED;
}

void GeoDataPointmap::TINManager::AddBreakLineController::deactivate(VTK2DGraphicsView* v)
{
	finish();
}

void GeoDataPointmap::TINManager::AddBreakLineController::handleKeyPressEvent(QKeyEvent* event, VTK2DGraphicsView* /*v*/)
{
	if (iRIC::isEnterKey(event->key())) {
		finish();
	}
}

void GeoDataPointmap::TINManager::AddBreakLineController::handleMouseDoubleClickEvent(QMouseEvent* /*event*/, VTK2DGraphicsView* /*v*/)
{
	finish();
}

void GeoDataPointmap::TINManager::AddBreakLineController::handleMouseMoveEvent(QMouseEvent* event, VTK2DGraphicsView* v)
{
	auto worldPos = v->viewportToWorld(event->pos());
	double radius = v->stdRadius(iRIC::nearRadius());
	vtkIdType newPoint = m_manager->impl->m_pointsManager->closestPointWithinRadius(worldPos, radius);

	if (newPoint != m_currentVertex) {
		m_manager->impl->m_parent->pushRenderCommand(new AddPointCommand(false, m_currentVertex != NOT_SELECTED, newPoint, this));
		m_currentVertex = newPoint;
	}

	if (m_currentVertex == NOT_SELECTED) {
		v->setCursor(Qt::ArrowCursor);
	} else {
		v->setCursor(m_addCursor);
	}
}

void GeoDataPointmap::TINManager::AddBreakLineController::handleMousePressEvent(QMouseEvent* event, VTK2DGraphicsView* v)
{
	if (m_currentVertex == NOT_SELECTED) {return;}

	auto indices = m_breakLine->vertexIndices();
	if (indices.size() >= 2 && *(indices.rbegin() + 1) == m_currentVertex) {return;}

	m_manager->impl->m_parent->pushModifyCommand(new AddPointCommand(true, true, m_currentVertex, this));
}

void GeoDataPointmap::TINManager::AddBreakLineController::finish()
{
	m_currentVertex = NOT_SELECTED;
	auto v = m_manager->impl->m_parent->dataModel()->graphicsView();
	v->setCursor(Qt::ArrowCursor);

	m_breakLine->controller().pointsActor()->VisibilityOff();
	m_breakLine->cleanVertexIndices();

	m_manager->m_actions->addBreakLineAction->setChecked(false);
	m_manager->impl->m_activeController = m_manager->impl->m_normalController;
	iRICUndoStack::instance().clear();

	if (m_breakLine->vertexIndices().size() < 2) {
		// it has not enough vertices. Delete it.
		m_breakLine->removeActorsFromRenderer(m_manager->impl->m_parent->renderer());
		m_manager->impl->m_breakLines.pop_back();
		delete m_breakLine;
	}
	m_manager->impl->m_parent->renderGraphicsView();
}
