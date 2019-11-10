#include "polygoncontroller.h"
#include "polygoneditcoordinatescommand.h"

#include <QPolygonF>

PolygonEditCoordinatesCommand::PolygonEditCoordinatesCommand(const std::vector<QPointF>& coords, PolygonController* polygon) :
	PolygonEditCoordinatesCommand {tr("Edit Polygon Coordinates"), coords, polygon}
{}

PolygonEditCoordinatesCommand::PolygonEditCoordinatesCommand(const QString& title, const std::vector<QPointF>& coords, PolygonController* polygon) :
	QUndoCommand {title},
	m_oldPolygon {polygon->polygon()},
	m_polygon {polygon}
{
	QPolygonF pol;
	for (const auto& p : coords) {
		pol.push_back(p);
	}
	pol.push_back(coords.at(0));
	m_newPolylgon = pol;
}

void PolygonEditCoordinatesCommand::redo()
{
	m_polygon->setPolygon(m_newPolylgon);
}

void PolygonEditCoordinatesCommand::undo()
{
	m_polygon->setPolygon(m_oldPolygon);
}
