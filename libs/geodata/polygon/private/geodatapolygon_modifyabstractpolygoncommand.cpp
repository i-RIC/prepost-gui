#include "geodatapolygon_modifyabstractpolygoncommand.h"
#include "../geodatapolygonabstractpolygon.h"

GeoDataPolygon::ModifyAbstractPolygonCommand::ModifyAbstractPolygonCommand(GeoDataPolygonAbstractPolygon* polygon, GeoDataPolygon* p, const QString& text) :
	QUndoCommand(text),
	m_oldPolygon {polygon->polygon()},
	m_wasMapped {p->isMapped()},
	m_targetPolygon {polygon},
	m_polygon {p}
{}

GeoDataPolygon::ModifyAbstractPolygonCommand::~ModifyAbstractPolygonCommand()
{}

void GeoDataPolygon::ModifyAbstractPolygonCommand::setNewPolygon(const QPolygonF& polygon)
{
	m_newPolygon = polygon;
}

bool GeoDataPolygon::ModifyAbstractPolygonCommand::mergeWith(const QUndoCommand *other)
{
	const ModifyAbstractPolygonCommand* comm = dynamic_cast<const ModifyAbstractPolygonCommand*> (other);
	if (comm == nullptr) {return false;}
	if (comm->m_polygon != m_polygon) {return false;}
	if (comm->m_targetPolygon != m_targetPolygon) {return false;}

	m_newPolygon = comm->m_newPolygon;
	return true;
}

void GeoDataPolygon::ModifyAbstractPolygonCommand::redo()
{
	m_polygon->setMapped(false);
	m_targetPolygon->setPolygon(m_newPolygon);
	m_polygon->updatePolyData();
}

void GeoDataPolygon::ModifyAbstractPolygonCommand::undo()
{
	m_polygon->setMapped(m_wasMapped);
	m_targetPolygon->setPolygon(m_oldPolygon);
	m_polygon->updatePolyData();
}
