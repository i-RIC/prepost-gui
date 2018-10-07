#include "geodatapolyline_modifyabstractpolylinecommand.h"
#include "../geodatapolylineimplpolyline.h"

GeoDataPolyLine::ModifyAbstractPolyLineCommand::ModifyAbstractPolyLineCommand(GeoDataPolyLine* p, const QString& text) :
	QUndoCommand(text),
	m_oldPolyLine (p->polyLine()->polyLine()),
	m_wasMapped {p->isMapped()},
	m_targetPolyLine {p->polyLine()},
	m_polyline {p}
{}

GeoDataPolyLine::ModifyAbstractPolyLineCommand::~ModifyAbstractPolyLineCommand()
{}

void GeoDataPolyLine::ModifyAbstractPolyLineCommand::setNewPolyLine(const std::vector<QPointF>& polyline)
{
	m_newPolyLine = polyline;
}

bool GeoDataPolyLine::ModifyAbstractPolyLineCommand::mergeWith(const QUndoCommand *other)
{
	const ModifyAbstractPolyLineCommand* comm = dynamic_cast<const ModifyAbstractPolyLineCommand*> (other);
	if (comm == nullptr) {return false;}
	if (comm->m_polyline != m_polyline) {return false;}
	if (comm->m_targetPolyLine != m_targetPolyLine) {return false;}

	m_newPolyLine = comm->m_newPolyLine;
	return true;
}

void GeoDataPolyLine::ModifyAbstractPolyLineCommand::redo()
{
	m_polyline->setMapped(false);
	m_targetPolyLine->setPolyLine(m_newPolyLine);
}

void GeoDataPolyLine::ModifyAbstractPolyLineCommand::undo()
{
	m_polyline->setMapped(m_wasMapped);
	m_targetPolyLine->setPolyLine(m_oldPolyLine);
}
