#include "../geodatariversurveybackgroundgridcreatethread.h"
#include "geodatariversurvey_deleteriverpathpointcommand.h"

GeoDataRiverSurvey::DeleteRiverPathPointCommand::DeleteRiverPathPointCommand(GeoDataRiverSurvey* rs) :
	QUndoCommand {GeoDataRiverSurvey::tr("Delete Traversal Lines")},
	m_rs {rs},
	m_redoed {false}
{
	GeoDataRiverPathPoint* p = m_rs->headPoint();
	while (p != nullptr) {
		if (p->IsSelected) {
			m_deletedPoints.append(p);
			m_beforePoints.append(p->previousPoint());
			m_prevSkips.append(p->previousPoint()->gridSkip());
			if (p->nextPoint() != nullptr) {
				m_nextSkips.append(p->nextPoint()->gridSkip());
			} else {
				m_nextSkips.append(false);
			}
		}
		p = p->nextPoint();
	}
}

GeoDataRiverSurvey::DeleteRiverPathPointCommand::~DeleteRiverPathPointCommand()
{
	if (! m_redoed) {return;}

	// remove the points.
	for (auto point : m_deletedPoints) {
		delete point;
	}
}

void GeoDataRiverSurvey::DeleteRiverPathPointCommand::redo()
{
	m_rs->m_gridThread->cancel();
	for (auto it = m_deletedPoints.begin(); it != m_deletedPoints.end(); ++it) {
		GeoDataRiverPathPoint* p = (*it);
		p->remove();
	}
	m_rs->headPoint()->updateRiverShapeInterpolators();
	m_rs->updateShapeData();
	m_rs->updateSelectionShapeData();
	m_rs->renderGraphicsView();
	m_rs->updateCrosssectionWindows();
	m_redoed = true;
	m_rs->setMapped(false);
}

void GeoDataRiverSurvey::DeleteRiverPathPointCommand::undo()
{
	m_rs->m_gridThread->cancel();
	for (int i = 0; i < m_deletedPoints.count(); ++i) {
		m_beforePoints[i]->addPathPoint(m_deletedPoints.at(i));
	}
	for (int i = 0; i < m_deletedPoints.count(); ++i) {
		GeoDataRiverPathPoint* p = m_deletedPoints[i];
		p->previousPoint()->setGridSkip(m_prevSkips[i]);
		if (p->nextPoint() != nullptr) {
			p->nextPoint()->setGridSkip(m_nextSkips[i]);
		}
	}
	m_rs->headPoint()->updateRiverShapeInterpolators();
	m_rs->updateShapeData();
	m_rs->updateSelectionShapeData();
	m_rs->renderGraphicsView();
	m_rs->updateCrosssectionWindows();
	m_redoed = false;
}
