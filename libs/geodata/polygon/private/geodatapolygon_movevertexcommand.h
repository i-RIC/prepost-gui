#ifndef GEODATAPOLYGON_MOVEVERTEXCOMMAND_H
#define GEODATAPOLYGON_MOVEVERTEXCOMMAND_H

#include "../geodatapolygon.h"
#include "geodatapolygon_modifyabstractpolygoncommand.h"

#include <QPolygonF>

class GeoDataPolygon::MoveVertexCommand : public GeoDataPolygon::ModifyAbstractPolygonCommand
{
public:
	MoveVertexCommand(bool keyDown, const QPoint& from, const QPoint& to, int vertexId, GeoDataPolygon* pol);

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
};

#endif // GEODATAPOLYGON_MOVEVERTEXCOMMAND_H
