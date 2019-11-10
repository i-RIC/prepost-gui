#include "geodatapolyline_movepolylinecommand.h"
#include "../geodatapolylineimplpolyline.h"

#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <misc/qundocommandhelper.h>

GeoDataPolyLine::MovePolyLineCommand::MovePolyLineCommand(bool keyDown, const QPoint& from, const QPoint& to, GeoDataPolyLine* pol) :
	QUndoCommand {GeoDataPolyLine::tr("Move PolyLine")}
{
	m_keyDown = keyDown;
	double dx = from.x();
	double dy = from.y();
	pol->graphicsView()->viewportToWorld(dx, dy);
	QPointF fromVec(dx, dy);
	dx = to.x();
	dy = to.y();
	pol->graphicsView()->viewportToWorld(dx, dy);
	QPointF toVec(dx, dy);
	m_offset = toVec - fromVec;
	m_oldMapped = pol->isMapped();
	m_polyline = pol;
}

void GeoDataPolyLine::MovePolyLineCommand::redo()
{
	m_polyline->setMapped(false);
	movePolyLine(m_polyline->polyLine(), m_offset);
}

void GeoDataPolyLine::MovePolyLineCommand::undo()
{
	m_polyline->setMapped(m_oldMapped);
	movePolyLine(m_polyline->polyLine(), - m_offset);
}

int GeoDataPolyLine::MovePolyLineCommand::id() const
{
	return iRIC::generateCommandId("GeoDataPolyLinePolyLineMove");
}

bool GeoDataPolyLine::MovePolyLineCommand::mergeWith(const QUndoCommand* other)
{
	const MovePolyLineCommand* comm = dynamic_cast<const MovePolyLineCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}
	if (comm->m_polyline != m_polyline) {return false;}
	m_offset += comm->m_offset;
	return true;
}

void GeoDataPolyLine::MovePolyLineCommand::movePolyLine(GeoDataPolyLineAbstractPolyLine* polyline, const QPointF& offset)
{
	auto newPol = polyline->polyLine(offset);
	polyline->setPolyLine(newPol);
}
