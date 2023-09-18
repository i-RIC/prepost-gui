#include "geodatariversurvey_mouserotaterivercrosssectioncommand.h"
#include "../geodatariversurveybackgroundgridcreatethread.h"

#include <guicore/pre/base/preprocessorgraphicsviewi.h>
#include <misc/mathsupport.h>
#include <misc/qundocommandhelper.h>

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
	PreProcessorGraphicsViewI* gview = data->graphicsView();
	m_rs = data;
	m_oldDirection = m_point->crosssectionDirection();

	auto fromWorld = gview->viewportToWorld(from);
	auto toWorld = gview->viewportToWorld(to);
	QPointF vec1 = QPointF(fromWorld.x() - m_point->position().x(), fromWorld.y() - m_point->position().y());
	QPointF vec2 = QPointF(toWorld.x() - m_point->position().x(), toWorld.y() - m_point->position().y());
	double angle = iRIC::angleRadian(vec1, vec2);

	m_newDirection = m_oldDirection;
	m_newDirection = iRIC::rotateVectorRadian(m_newDirection, angle);
}

void GeoDataRiverSurvey::MouseRotateRiverCrosssectionCommand::redo()
{
	m_rs->cancelBackgroundGridUpdate();
	m_point->setCrosssectionDirection(m_newDirection);
	m_rs->updateShapeData();
	m_rs->setMapped(false);
}

void GeoDataRiverSurvey::MouseRotateRiverCrosssectionCommand::undo()
{
	m_rs->cancelBackgroundGridUpdate();
	m_point->setCrosssectionDirection(m_oldDirection);
	m_rs->updateShapeData();
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
