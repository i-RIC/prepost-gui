#include "geodatariversurvey_changeselectioncommand.h"

#include <guicore/misc/mouseboundingbox.h>

GeoDataRiverSurvey::ChangeSelectionCommand::ChangeSelectionCommand(GeoDataRiverSurvey* rs, MouseBoundingBox* box) :
	QUndoCommand {GeoDataRiverSurvey::tr("Selection Change")},
	m_rs {rs}
{
	// store old selection info.
	buildSelectedPointsSet(m_oldSelectedPoints);

	// now, update the selection statuses of river path points.
	double point[3];
	vtkPoints* points = box->vtkGrid()->GetPoints();
	QVector2D leftTop, rightTop, leftBottom;
	// left top
	points->GetPoint(0, point);
	leftTop = QVector2D(point[0], point[1]);
	// right top
	points->GetPoint(1, point);
	rightTop = QVector2D(point[0], point[1]);
	// left bottom
	points->GetPoint(3, point);
	leftBottom = QVector2D(point[0], point[1]);

	// do the selection!
	m_rs->headPoint()->selectRegion(leftTop, rightTop - leftTop, leftBottom - leftTop);

	// store new selection info.
	buildSelectedPointsSet(m_newSelectedPoints);
}

void GeoDataRiverSurvey::ChangeSelectionCommand::redo()
{
	applySelection(m_newSelectedPoints);
}

void GeoDataRiverSurvey::ChangeSelectionCommand::undo()
{
	applySelection(m_oldSelectedPoints);
}

void GeoDataRiverSurvey::ChangeSelectionCommand::applySelection(const QSet<GeoDataRiverPathPoint*>& selected)
{
	GeoDataRiverPathPoint* p = m_rs->headPoint();
	if (p != nullptr) {p = p->nextPoint();}
	while (p != nullptr) {
		p->IsSelected = selected.contains(p);
		p = p->nextPoint();
	}
	m_rs->updateActionStatus();
	m_rs->updateSelectionShapeData();
	m_rs->renderGraphicsView();
}

void GeoDataRiverSurvey::ChangeSelectionCommand::buildSelectedPointsSet(QSet<GeoDataRiverPathPoint*>& set)
{
	set.clear();
	GeoDataRiverPathPoint* p = m_rs->headPoint();
	if (p != nullptr) {p = p->nextPoint();}
	while (p != nullptr) {
		if (p->IsSelected) {
			set.insert(p);
		}
		p = p->nextPoint();
	}
}
