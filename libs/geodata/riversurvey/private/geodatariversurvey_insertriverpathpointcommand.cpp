#include "geodatariversurvey_insertriverpathpointcommand.h"
#include "../geodatariversurveybackgroundgridcreatethread.h"

GeoDataRiverSurvey::InsertRiverPathPointCommand::InsertRiverPathPointCommand(bool apply, GeoDataRiverPathPoint* prev, GeoDataRiverPathPoint* newpoint, GeoDataRiverSurvey* rs) :
	QUndoCommand {GeoDataRiverSurvey::tr("Insert Traversal Line")},
	m_previousPoint {prev},
	m_newPoint {newpoint},
	m_rs {rs},
	m_apply {apply},
	m_redoed {false}
{}

GeoDataRiverSurvey::InsertRiverPathPointCommand::~InsertRiverPathPointCommand() {
	if ((! m_redoed) && (! m_apply)) {
		delete m_newPoint;
	}
}

void GeoDataRiverSurvey::InsertRiverPathPointCommand::redo()
{
	m_rs->cancelBackgroundGridUpdate();
	m_previousPoint->addPathPoint(m_newPoint);
	m_previousPoint->UpdateCtrlSections();
	m_rs->updateSplineSolvers();
	m_rs->updateShapeData();
	m_rs->updateSelectionShapeData();
	m_rs->renderGraphicsView();
	m_rs->updateCrosssectionWindows();
	m_redoed = true;
}

void GeoDataRiverSurvey::InsertRiverPathPointCommand::undo()
{
	m_rs->cancelBackgroundGridUpdate();
	m_newPoint->remove();
	m_previousPoint->UpdateCtrlSections();
	m_rs->updateSplineSolvers();
	if (! m_apply) {
		m_rs->updateShapeData();
		m_rs->updateSelectionShapeData();
		m_rs->renderGraphicsView();
		m_rs->updateCrosssectionWindows();
	}
	m_redoed = false;
}
