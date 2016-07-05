#include "geodatapolygon_addvertexcommand.h"
#include "../geodatapolygonabstractpolygon.h"

#include <guicore/misc/qundocommandhelper.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>

GeoDataPolygon::AddVertexCommand::AddVertexCommand(bool keyDown, vtkIdType edgeId, QPoint point, GeoDataPolygon* pol) :
	GeoDataPolygon::ModifyAbstractPolygonCommand(pol->m_selectedPolygon, pol, GeoDataPolygon::tr("Insert Polygon Vertex")),
	m_keyDown {keyDown}
{
	auto vertexId = (edgeId + 1) % (pol->m_selectedPolygon->getVtkPolygon()->GetNumberOfPoints() + 1);
	double dx = point.x();
	double dy = point.y();
	pol->graphicsView()->viewportToWorld(dx, dy);
	QPointF newPoint(dx, dy);

	QPolygonF newPolygon(pol->m_selectedPolygon->polygon(QPointF(0, 0), true));

	if (keyDown) {
		newPolygon.insert(vertexId, newPoint);
		if (vertexId == 0) {
			newPolygon.pop_back();
			newPolygon.push_back(newPoint);
		}
	}	else {
		newPolygon[vertexId] = newPoint;
	}
	setNewPolygon(newPolygon);
}

int GeoDataPolygon::AddVertexCommand::id() const
{
	return iRIC::generateCommandId("GeoDataPolygonPolygonAddVertex");
}

bool GeoDataPolygon::AddVertexCommand::mergeWith(const QUndoCommand* other)
{
	const AddVertexCommand* comm = dynamic_cast<const AddVertexCommand*>(other);
	if (comm == nullptr) {return false;}
	if (comm->m_keyDown) {return false;}

	return ModifyAbstractPolygonCommand::mergeWith(other);
}
