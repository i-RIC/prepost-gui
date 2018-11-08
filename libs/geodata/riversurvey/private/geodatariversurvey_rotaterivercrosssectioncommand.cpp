#include "geodatariversurvey_rotaterivercrosssectioncommand.h"
#include "../geodatariversurveybackgroundgridcreatethread.h"

#include <guicore/misc/qundocommandhelper.h>
#include <misc/mathsupport.h>

GeoDataRiverSurvey::RotateRiverCrosssectionCommand::RotateRiverCrosssectionCommand(bool apply, double angle, GeoDataRiverSurvey* rs) :
	QUndoCommand {GeoDataRiverSurvey::tr("Rotate Traversal Line")},
	m_apply {apply},
	m_rs {rs}
{
	GeoDataRiverPathPoint* p = rs->headPoint();
	p = p->nextPoint();
	while (p != nullptr) {
		if (p->IsSelected) {
			m_point = p;
			break;
		}
		p = p->nextPoint();
	}
	m_oldDirection = m_point->crosssectionDirection();
	m_newDirection = m_oldDirection;
	iRIC::rotateVector(m_newDirection, angle);
}

void GeoDataRiverSurvey::RotateRiverCrosssectionCommand::redo()
{
	m_rs->m_gridThread->cancel();

	m_point->setCrosssectionDirection(m_newDirection);

	m_rs->setModified();
	m_rs->updateShapeData();
	m_rs->renderGraphicsView();
}

void GeoDataRiverSurvey::RotateRiverCrosssectionCommand::undo()
{
	m_rs->m_gridThread->cancel();

	m_point->setCrosssectionDirection(m_oldDirection);
	if (! m_apply) {
		m_rs->updateShapeData();
		m_rs->renderGraphicsView();
	}
}
