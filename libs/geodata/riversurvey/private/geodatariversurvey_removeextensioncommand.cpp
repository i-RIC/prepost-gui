#include "geodatariversurvey_impl.h"
#include "geodatariversurvey_removeextensioncommand.h"
#include "../geodatariversurveybackgroundgridcreatethread.h"

GeoDataRiverSurvey::RemoveExtensionCommand::RemoveExtensionCommand(bool left, const QPointF &pos, GeoDataRiverPathPoint* p, GeoDataRiverSurvey* rs) :
	QUndoCommand {GeoDataRiverSurvey::tr("Remove Extension Line")},
	m_point {p},
	m_rs {rs},
	m_left {left},
	m_position {pos}
{}

void GeoDataRiverSurvey::RemoveExtensionCommand::redo()
{
	m_rs->cancelBackgroundGridUpdate();
	if (m_left) {
		m_point->removeExtentionPointLeft();
	} else {
		m_point->removeExtentionPointRight();
	}
	updateData();
	m_rs->setMapped(false);
}

void GeoDataRiverSurvey::RemoveExtensionCommand::undo()
{
	m_rs->cancelBackgroundGridUpdate();
	if (m_left) {
		m_point->addExtentionPointLeft(m_position);
	} else {
		m_point->addExtentionPointRight(m_position);
	}
	updateData();
}

void GeoDataRiverSurvey::RemoveExtensionCommand::updateData()
{
	m_rs->impl->updateActionStatus();
	m_rs->headPoint()->updateAllXSecInterpolators();
	m_rs->headPoint()->updateRiverShapeInterpolators();
	m_rs->updateShapeData();
	m_rs->renderGraphicsView();
	m_rs->updateCrosssectionWindows();
}
