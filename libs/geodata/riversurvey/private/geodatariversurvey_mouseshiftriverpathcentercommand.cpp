#include "geodatariversurvey_mouseshiftriverpathcentercommand.h"
#include "../geodatariversurveybackgroundgridcreatethread.h"

#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <misc/qundocommandhelper.h>

GeoDataRiverSurvey::MouseShiftRiverPathCenterCommand::MouseShiftRiverPathCenterCommand(QPoint from, QPoint to, GeoDataRiverSurvey* data) :
	QUndoCommand {GeoDataRiverSurvey::tr("Shift Center Point")},
	m_rs {data}
{
	PreProcessorGraphicsViewInterface* gview = data->graphicsView();
	double fromX, fromY, toX, toY;
	fromX = from.x();
	fromY = from.y();
	toX = to.x();
	toY = to.y();
	gview->viewportToWorld(fromX, fromY);
	gview->viewportToWorld(toX, toY);
	QPointF offset(toX - fromX, toY - fromY);
	GeoDataRiverPathPoint* first = nullptr;

	GeoDataRiverPathPoint* p = data->m_headPoint->nextPoint();
	while (p != nullptr) {
		if (p->IsSelected) {
			m_points.append(p);
			if (first == nullptr) {first = p;}
		}
		p = p->nextPoint();
	}
	m_shiftValue = QPointF::dotProduct(offset, first->crosssectionDirection());
}

void GeoDataRiverSurvey::MouseShiftRiverPathCenterCommand::redo()
{
	m_rs->cancelBackgroundGridUpdate();
	for (int i = 0; i < m_points.count(); ++i) {
		m_points[i]->shiftCenter(m_shiftValue);
	}
	m_rs->headPoint()->updateAllXSecInterpolators();
	m_rs->headPoint()->updateRiverShapeInterpolators();
	m_rs->updateShapeData();
	m_rs->renderGraphicsView();
	m_rs->updateCrosssectionWindows();
	m_rs->setMapped(false);
}

void GeoDataRiverSurvey::MouseShiftRiverPathCenterCommand::undo()
{
	m_rs->cancelBackgroundGridUpdate();
	for (int i = 0; i < m_points.count(); ++i) {
		m_points[i]->shiftCenter(- m_shiftValue);
	}
	m_rs->headPoint()->updateAllXSecInterpolators();
	m_rs->headPoint()->updateRiverShapeInterpolators();
	m_rs->updateShapeData();
	m_rs->renderGraphicsView();
	m_rs->updateCrosssectionWindows();
}

int GeoDataRiverSurvey::MouseShiftRiverPathCenterCommand::id() const
{
	return iRIC::generateCommandId("GeoDataRiverSurveyMouseShift");
}

bool GeoDataRiverSurvey::MouseShiftRiverPathCenterCommand::mergeWith(const QUndoCommand* other)
{
	const MouseShiftRiverPathCenterCommand* other2 = dynamic_cast<const MouseShiftRiverPathCenterCommand*>(other);
	if (other2 == nullptr) { return false; }
	if (other2->m_points == m_points) {
		m_shiftValue += other2->m_shiftValue;
		return true;
	}
	return false;
}
