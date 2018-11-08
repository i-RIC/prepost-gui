#include "geodatariversurvey_addextensioncommand.h"
#include "../geodatariversurveybackgroundgridcreatethread.h"

GeoDataRiverSurvey::AddExtensionCommand::AddExtensionCommand(bool apply, GeoDataRiverPathPointExtensionAddDialog::LineMode lm, const QPointF& pos, GeoDataRiverPathPoint* p, GeoDataRiverSurvey* rs) :
	QUndoCommand {GeoDataRiverSurvey::tr("Add Extension Line")},
	m_apply {apply},
	m_lineMode {lm},
	m_position {pos},
	m_point {p},
	m_rs {rs}
{}

void GeoDataRiverSurvey::AddExtensionCommand::redo()
{
	m_rs->m_gridThread->cancel();
	if (m_lineMode == GeoDataRiverPathPointExtensionAddDialog::Left) {
		m_point->addExtentionPointLeft(m_position);
	} else {
		m_point->addExtentionPointRight(m_position);
	}
	m_rs->updateActionStatus();
	m_rs->headPoint()->updateAllXSecInterpolators();
	m_rs->headPoint()->updateRiverShapeInterpolators();
	m_rs->setModified();
	m_rs->updateShapeData();
	m_rs->renderGraphicsView();
	m_rs->updateCrossectionWindows();
}

void GeoDataRiverSurvey::AddExtensionCommand::undo()
{
	m_rs->m_gridThread->cancel();
	if (m_lineMode == GeoDataRiverPathPointExtensionAddDialog::Left) {
		m_point->removeExtentionPointLeft();
	} else {
		m_point->removeExtentionPointRight();
	}
	if (! m_apply) {
		m_rs->updateActionStatus();
		m_rs->headPoint()->updateAllXSecInterpolators();
		m_rs->headPoint()->updateRiverShapeInterpolators();
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
		m_rs->updateCrossectionWindows();
	}
}
