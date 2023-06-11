#include "../geodatapointmap.h"
#include "geodatapointmap_tinmanager_actions.h"
#include "geodatapointmap_tinmanager_breakline.h"
#include "geodatapointmap_tinmanager_deletebreaklinecontroller.h"
#include "geodatapointmap_tinmanager_impl.h"

#include <guicore/datamodel/vtk2dgraphicsview.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <misc/informationdialog.h>
#include <misc/mathsupport.h>

GeoDataPointmap::TINManager::DeleteBreakLineController::DeleteBreakLineController(TINManager* manager) :
	m_removePixmap {":/libs/guibase/images/cursorRemove.png"},
	m_removeCursor {m_removePixmap, 0, 0},
	m_deleteTarget {nullptr},
	m_manager {manager}
{}

void GeoDataPointmap::TINManager::DeleteBreakLineController::activate(VTK2DGraphicsView* v)
{
	auto w = m_manager->impl->m_parent->preProcessorWindow();
	InformationDialog::information(w, GeoDataPointmap::tr("Information"), GeoDataPointmap::tr("Click on the break line you want to remove."), "geodatapointmapremovebreakline");

	m_deleteTarget = nullptr;
}

void GeoDataPointmap::TINManager::DeleteBreakLineController::deactivate(VTK2DGraphicsView* v)
{
	v->setCursor(Qt::ArrowCursor);
	m_deleteTarget = nullptr;
	m_manager->m_actions->removeBreakLineAction->setChecked(false);
}

void GeoDataPointmap::TINManager::DeleteBreakLineController::handleMouseMoveEvent(QMouseEvent* event, VTK2DGraphicsView* v)
{
	m_deleteTarget = nullptr;

	auto worldPos = v->viewportToWorld(event->pos());
	auto limitDist = v->stdRadius(iRIC::nearRadius());

	for (auto line : m_manager->impl->m_breakLines) {
		line->controller().pointsActor()->VisibilityOff();
	}

	for (auto line : m_manager->impl->m_breakLines) {
		int edgeId;
		if (line->controller().isEdgeSelectable(worldPos, limitDist, &edgeId)) {
			line->controller().pointsActor()->VisibilityOn();
			m_deleteTarget = line;
			break;
		}
	}
	if (m_deleteTarget == nullptr) {
		v->setCursor(Qt::ArrowCursor);
	} else {
		v->setCursor(m_removeCursor);
	}
	m_manager->impl->m_parent->renderGraphicsView();
}

void GeoDataPointmap::TINManager::DeleteBreakLineController::handleMousePressEvent(QMouseEvent* event, VTK2DGraphicsView* v)
{
	if (m_deleteTarget == nullptr) {return;}
	if (event->button() != Qt::LeftButton) {return;}

	auto w = m_manager->impl->m_parent->preProcessorWindow();
	int ret = QMessageBox::warning(w, GeoDataPointmap::tr("Warning"),
																 GeoDataPointmap::tr("Are you sure you want to remove this break line?"),
																 QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (ret == QMessageBox::No) {return;}

	m_manager->removeBreakLine(m_deleteTarget);
	deactivate(v);
	m_manager->impl->m_activeController = m_manager->impl->m_normalController;
}
