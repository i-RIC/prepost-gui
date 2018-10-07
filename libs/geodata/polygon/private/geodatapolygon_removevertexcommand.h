#ifndef GEODATAPOLYGON_REMOVEVERTEXCOMMAND_H
#define GEODATAPOLYGON_REMOVEVERTEXCOMMAND_H

#include "geodatapolygon_modifyabstractpolygoncommand.h"
#include "../geodatapolygon.h"

class GeoDataPolygon::RemoveVertexCommand : public GeoDataPolygon::ModifyAbstractPolygonCommand
{
public:
	RemoveVertexCommand(int vertexId, GeoDataPolygon* pol);
};

#endif // GEODATAPOLYGON_REMOVEVERTEXCOMMAND_H
