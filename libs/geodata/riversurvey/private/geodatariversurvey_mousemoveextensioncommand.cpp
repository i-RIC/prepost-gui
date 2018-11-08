#include "geodatariversurvey_mousemoveextensioncommand.h"
#include "../geodatariversurveybackgroundgridcreatethread.h"

#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>

GeoDataRiverSurvey::MouseMoveExtensionCommand::MouseMoveExtensionCommand(bool left, QPoint to, GeoDataRiverSurvey* data) :
	QUndoCommand {GeoDataRiverSurvey::tr("Move Extension Line End")},
	m_left {left},
	m_rs {data}
{
	PreProcessorGraphicsViewInterface* gview = data->graphicsView();
	double toX, toY;
	toX = to.x();
	toY = to.y();
	gview->viewportToWorld(toX, toY);

	GeoDataRiverPathPoint* p = data->m_headPoint->nextPoint();
	while (p != nullptr) {
		if (p->IsSelected) {
			m_point = p;
			break;
		}
		p = p->nextPoint();
	}
	m_newPosition = QVector2D(toX, toY);
	if (m_left) {
		m_oldPosition = m_point->crosssectionPosition(m_point->crosssection().leftBank(true).position());
	} else {
		m_oldPosition = m_point->crosssectionPosition(m_point->crosssection().rightBank(true).position());
	}
}

void GeoDataRiverSurvey::MouseMoveExtensionCommand::redo()
{
	applyPosition(m_newPosition);
	m_rs->setMapped(false);
}

void GeoDataRiverSurvey::MouseMoveExtensionCommand::undo()
{
	applyPosition(m_oldPosition);
}

int GeoDataRiverSurvey::MouseMoveExtensionCommand::id() const
{
	return iRIC::generateCommandId("GeoDataRiverPathPointMouseMoveExtension");
}

bool GeoDataRiverSurvey::MouseMoveExtensionCommand::mergeWith(const QUndoCommand* other)
{
	const MouseMoveExtensionCommand* other2 = dynamic_cast<const MouseMoveExtensionCommand*>(other);
	if (other2 == nullptr) { return false; }
	if (other2->m_point == m_point && other2->m_left == m_left) {
		m_newPosition = other2->m_newPosition;
		return true;
	}
	return false;
}

void GeoDataRiverSurvey::MouseMoveExtensionCommand::applyPosition(const QVector2D& pos)
{
	m_rs->m_gridThread->cancel();
	if (m_left) {
		m_point->moveExtentionPointLeft(pos);
	} else {
		m_point->moveExtentionPointRight(pos);
	}
	m_rs->headPoint()->updateAllXSecInterpolators();
	m_rs->headPoint()->updateRiverShapeInterpolators();
	m_rs->updateShapeData();
	m_rs->renderGraphicsView();
	m_rs->updateCrossectionWindows();
}
