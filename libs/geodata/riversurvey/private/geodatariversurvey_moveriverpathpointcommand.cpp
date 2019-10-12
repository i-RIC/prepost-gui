#include "geodatariversurvey_moveriverpathpointcommand.h"
#include "../geodatariversurveybackgroundgridcreatethread.h"

GeoDataRiverSurvey::MoveRiverPathPointCommand::MoveRiverPathPointCommand(bool apply, const QPointF& offset, GeoDataRiverSurvey* rs) :
	QUndoCommand {GeoDataRiverSurvey::tr("Move Traversal Lines")}
{
	m_apply = apply;
	GeoDataRiverPathPoint* p = rs->headPoint();
	p = p->nextPoint();
	while (p != 0) {
		if (p->IsSelected) {
			m_points.append(p);
			m_oldPositions.append(p->position());
			m_newPositions.append(p->position() + offset);
		}
		p = p->nextPoint();
	}
	m_rs = rs;
}

void GeoDataRiverSurvey::MoveRiverPathPointCommand::redo()
{
	m_rs->m_gridThread->cancel();

	for (int i = 0; i < m_points.count(); ++i) {
		auto newpos = m_newPositions.at(i);
		m_points[i]->setPosition(newpos);
	}
	m_rs->headPoint()->updateRiverShapeInterpolators();
	m_rs->setModified();
	m_rs->updateShapeData();
	m_rs->renderGraphicsView();
	m_rs->updateCrosssectionWindows();
}

void GeoDataRiverSurvey::MoveRiverPathPointCommand::undo()
{
	m_rs->m_gridThread->cancel();

	for (int i = 0; i < m_points.count(); ++i) {
		auto oldpos = m_oldPositions.at(i);
		m_points[i]->setPosition(oldpos);
	}
	if (! m_apply) {
		m_rs->headPoint()->updateRiverShapeInterpolators();
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
		m_rs->updateCrosssectionWindows();
	}
}

