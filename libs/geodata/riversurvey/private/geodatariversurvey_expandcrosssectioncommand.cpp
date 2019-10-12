#include "geodatariversurvey_expandcrosssectioncommand.h"
#include "../geodatariversurveybackgroundgridcreatethread.h"

GeoDataRiverSurvey::ExpandCrosssectionCommand::ExpandCrosssectionCommand(bool apply, QList<GeoDataRiverPathPoint*>& points, QList<double> ratios, GeoDataRiverSurvey* rs) :
	QUndoCommand {GeoDataRiverSurvey::tr("Expand Traversal Lines")},
	m_apply {apply},
	m_points (points),
	m_rs {rs}
{
	for (int i = 0; i < points.count(); ++i) {
		m_oldCrosssections.append(points[i]->crosssection().AltitudeInfo());
		m_points[i]->crosssection().expand(ratios[i]);
		m_newCrosssections.append(points[i]->crosssection().AltitudeInfo());
	}
}

void GeoDataRiverSurvey::ExpandCrosssectionCommand::redo()
{
	m_rs->m_gridThread->cancel();
	for (int i = 0; i < m_points.count(); ++i) {
		m_points[i]->crosssection().AltitudeInfo() = m_newCrosssections[i];
	}
	m_rs->headPoint()->updateAllXSecInterpolators();
	m_rs->headPoint()->updateRiverShapeInterpolators();
	m_rs->setModified();
	m_rs->updateShapeData();
	m_rs->renderGraphicsView();
	m_rs->updateCrosssectionWindows();
}

void GeoDataRiverSurvey::ExpandCrosssectionCommand::undo()
{
	m_rs->m_gridThread->cancel();
	for (int i = 0; i < m_points.count(); ++i) {
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
