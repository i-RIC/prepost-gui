#ifndef GEODATAPOLYLINE_REMOVEVERTEXCOMMAND_H
#define GEODATAPOLYLINE_REMOVEVERTEXCOMMAND_H

#include "geodatapolyline_modifyabstractpolylinecommand.h"
#include "../geodatapolyline.h"

class GeoDataPolyLine::RemoveVertexCommand : public GeoDataPolyLine::ModifyAbstractPolyLineCommand
{
public:
	RemoveVertexCommand(int vertexId, GeoDataPolyLine* pol);
};

#endif // GEODATAPOLYLINE_REMOVEVERTEXCOMMAND_H
