#include "geodatariversurvey_shiftriverpathcentercommand.h"
#include "../geodatariversurveybackgroundgridcreatethread.h"

GeoDataRiverSurvey::ShiftRiverPathCenterCommand::ShiftRiverPathCenterCommand(bool apply, double offset, GeoDataRiverSurvey* rs) :
	QUndoCommand {GeoDataRiverSurvey::tr("Shift River Center Points")},
	m_apply {apply},
	m_rs {rs}
{
	GeoDataRiverPathPoint* p = rs->headPoint();
	p = p->nextPoint();
	while (p != nullptr) {
		if (p->IsSelected) {
			m_points.append(p);
			m_oldPositions.append(p->position());
			m_oldCrosssections.append(p->crosssection().AltitudeInfo());
			p->shiftCenter(offset);
			m_newPositions.append(p->position());
			m_newCrosssections.append(p->crosssection().AltitudeInfo());
		}
		p = p->nextPoint();
	}
}

void GeoDataRiverSurvey::ShiftRiverPathCenterCommand::redo()
{
	m_rs->cancelBackgroundGridUpdate();
	for (int i = 0; i < m_points.count(); ++i) {
		m_points[i]->setPosition(m_newPositions[i]);
		m_points[i]->crosssection().AltitudeInfo() = m_newCrosssections[i];
	}
	m_rs->headPoint()->updateAllXSecInterpolators();
	m_rs->headPoint()->updateRiverShapeInterpolators();
	m_rs->setModified();
	m_rs->updateShapeData();
	m_rs->renderGraphicsView();
	m_rs->updateCrosssectionWindows();
}

void GeoDataRiverSurvey::ShiftRiverPathCenterCommand::undo()
{
	m_rs->cancelBackgroundGridUpdate();
	for (int i = 0; i < m_points.count(); ++i) {
		m_points[i]->setPosition(m_oldPositions[i]);
		m_points[i]->crosssection().AltitudeInfo() = m_oldCrosssections[i];
	}
	if (! m_apply) {
		m_rs->headPoint()->updateAllXSecInterpolators();
		m_rs->headPoint()->updateRiverShapeInterpolators();
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
		m_rs->updateCrosssectionWindows();
	}
}
