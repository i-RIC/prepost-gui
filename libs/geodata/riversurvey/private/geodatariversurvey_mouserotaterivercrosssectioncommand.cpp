#include "geodatariversurvey_mouserotaterivercrosssectioncommand.h"
#include "../geodatariversurveybackgroundgridcreatethread.h"

#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <misc/mathsupport.h>

GeoDataRiverSurvey::MouseRotateRiverCrosssectionCommand::MouseRotateRiverCrosssectionCommand(QPoint from, QPoint to, GeoDataRiverSurvey* data) :
	QUndoCommand {GeoDataRiverSurvey::tr("Rotate Traversal Line")}
{
	GeoDataRiverPathPoint* p = data->m_headPoint->nextPoint();
	while (p != nullptr) {
		if (p->IsSelected) {
			m_point = p;
			break;
		}
		p = p->nextPoint();
	}
	PreProcessorGraphicsViewInterface* gview = data->graphicsView();
	m_rs = data;
	m_oldDirection = m_point->crosssectionDirection();

	double fromX, fromY, toX, toY;
	fromX = from.x();
	fromY = from.y();
	toX = to.x();
	toY = to.y();
	gview->viewportToWorld(fromX, fromY);
	gview->viewportToWorld(toX, toY);
	QPointF vec1 = QPointF(fromX - m_point->position().x(), fromY - m_point->position().y());
	QPointF vec2 = QPointF(toX - m_point->position().x(), toY - m_point->position().y());
	double angle = iRIC::angleRadian(vec1, vec2);

	m_newDirection = m_oldDirection;
	iRIC::rotateVectorRadian(m_newDirection, angle);
}

void GeoDataRiverSurvey::MouseRotateRiverCrosssectionCommand::redo()
{
	m_rs->m_gridThread->cancel();
	m_point->setCrosssectionDirection(m_newDirection);
	m_rs->updateShapeData();
	m_rs->renderGraphicsView();
	m_rs->setMapped(false);
}

void GeoDataRiverSurvey::MouseRotateRiverCrosssectionCommand::undo()
{
	m_rs->m_gridThread->cancel();
	m_point->setCrosssectionDirection(m_oldDirection);
	m_rs->updateShapeData();
	m_rs->renderGraphicsView();
}

int GeoDataRiverSurvey::MouseRotateRiverCrosssectionCommand::id() const
{
	return iRIC::generateCommandId("GeoDataRiverSurveyMouseRotate");
}

bool GeoDataRiverSurvey::MouseRotateRiverCrosssectionCommand::mergeWith(const QUndoCommand* other)
{
	const MouseRotateRiverCrosssectionCommand* other2 = dynamic_cast<const MouseRotateRiverCrosssectionCommand*>(other);
	if (other2 == nullptr) { return false; }
	if (other2->m_point == m_point) {
		m_newDirection = other2->m_newDirection;
		return true;
	}
	return false;
}
