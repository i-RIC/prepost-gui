#include "geodatapolyline_modifyabstractpolylinecommand.h"
#include "geodatapolyline_removevertexcommand.h"
#include "../geodatapolylineimplpolyline.h"

#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>

GeoDataPolyLine::RemoveVertexCommand::RemoveVertexCommand(int vertexId, GeoDataPolyLine* pol) :
	GeoDataPolyLine::ModifyAbstractPolyLineCommand(pol, GeoDataPolyLine::tr("Remove PolyLine Vertex"))
{
	auto newPolyLine = pol->polyLine()->polyLine();
	newPolyLine.erase(newPolyLine.begin() + vertexId);
	setNewPolyLine(newPolyLine);
}
