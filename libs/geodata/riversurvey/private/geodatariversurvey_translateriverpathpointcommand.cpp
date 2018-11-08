#include "geodatariversurvey_translateriverpathpointcommand.h"
#include "../geodatariversurveybackgroundgridcreatethread.h"

#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>

GeoDataRiverSurvey::TranslateRiverPathPointCommand::TranslateRiverPathPointCommand(QPoint from, QPoint to, GeoDataRiverSurvey* data) :
	QUndoCommand {GeoDataRiverSurvey::tr("Move Traversal Line")},
	m_rs {data}
{
	PreProcessorGraphicsViewInterface* gview = data->graphicsView();
	double fromX, fromY, toX, toY;
	fromX = from.x();
	fromY = from.y();
	toX = to.x();
	toY = to.y();
	gview->viewportToWorld(fromX, fromY);
	gview->viewportToWorld(toX, toY);
	QVector2D offset(toX - fromX, toY - fromY);

	GeoDataRiverPathPoint* p = data->m_headPoint->nextPoint();
	while (p != nullptr) {
		if (p->IsSelected) {
			m_points.append(p);
			m_oldPositions.append(p->position());
			m_newPositions.append(p->position() + offset);
		}
		p = p->nextPoint();
	}
}

void GeoDataRiverSurvey::TranslateRiverPathPointCommand::redo()
{
	m_rs->m_gridThread->cancel();
	for (int i = 0; i < m_points.count(); ++i) {
		m_points[i]->setPosition(m_newPositions[i]);
	}
	m_rs->headPoint()->updateRiverShapeInterpolators();
	m_rs->updateShapeData();
	m_rs->renderGraphicsView();
	m_rs->updateCrossectionWindows();
	m_rs->setMapped(false);
}

void GeoDataRiverSurvey::TranslateRiverPathPointCommand::undo()
{
	m_rs->m_gridThread->cancel();
	for (int i = 0; i < m_points.count(); ++i) {
		m_points[i]->setPosition(m_oldPositions[i]);
	}
	m_rs->headPoint()->updateRiverShapeInterpolators();
	m_rs->updateShapeData();
	m_rs->renderGraphicsView();
	m_rs->updateCrossectionWindows();
}

int GeoDataRiverSurvey::TranslateRiverPathPointCommand::id() const
{
	return iRIC::generateCommandId("GeoDataRiverSurveyTranslateCommand");
}

bool GeoDataRiverSurvey::TranslateRiverPathPointCommand::mergeWith(const QUndoCommand* other)
{
	const TranslateRiverPathPointCommand* other2 = dynamic_cast<const TranslateRiverPathPointCommand*>(other);
	if (other2 == nullptr) { return false; }
	if (other2->m_points == m_points) {
		m_newPositions = other2->m_newPositions;
		return true;
	}
	return false;
}
