#include "geodatapolyline_addvertexcommand.h"
#include "../geodatapolylineimplpolyline.h"

#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>

#include <vtkPolyLine.h>

GeoDataPolyLine::AddVertexCommand::AddVertexCommand(bool keyDown, int edgeId, QPoint point, GeoDataPolyLine* pol) :
	GeoDataPolyLine::ModifyAbstractPolyLineCommand(pol, GeoDataPolyLine::tr("Insert PolyLine Vertex")),
	m_keyDown {keyDown}
{
	auto vertexId = edgeId + 1;
	double dx = point.x();
	double dy = point.y();
	pol->graphicsView()->viewportToWorld(dx, dy);
	QPointF newPoint(dx, dy);

	auto newPolyLine = pol->polyLine()->polyLine();

	if (keyDown) {
		newPolyLine.insert(newPolyLine.begin() + vertexId, newPoint);
	}	else {
		newPolyLine[vertexId] = newPoint;
	}
	setNewPolyLine(newPolyLine);
}

int GeoDataPolyLine::AddVertexCommand::id() const
{
	return iRIC::generateCommandId("GeoDataPolyLinePolyLineAddVertex");
}

bool GeoDataPolyLine::AddVertexCommand::mergeWith(const QUndoCommand* other)
{
	const AddVertexCommand* comm = dynamic_cast<const AddVertexCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}

	return ModifyAbstractPolyLineCommand::mergeWith(other);
}
