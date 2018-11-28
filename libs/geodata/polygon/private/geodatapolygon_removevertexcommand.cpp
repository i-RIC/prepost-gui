#include "geodatapolygon_modifyabstractpolygoncommand.h"
#include "geodatapolygon_removevertexcommand.h"
#include "../geodatapolygonabstractpolygon.h"

#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>

GeoDataPolygon::RemoveVertexCommand::RemoveVertexCommand(int vertexId, GeoDataPolygon* pol) :
	GeoDataPolygon::ModifyAbstractPolygonCommand(pol->selectedPolygon(), pol, GeoDataPolygon::tr("Remove Polygon Vertex"))
{
	QPolygonF newPolygon = pol->selectedPolygon()->polygon();
	newPolygon.removeAt(vertexId);
	if (vertexId == 0) {
		newPolygon.removeLast();
		newPolygon.push_back(newPolygon.at(0));
	}
	setNewPolygon(newPolygon);
}
