#ifndef GEODATAPOLYGON_ADDVERTEXCOMMAND_H
#define GEODATAPOLYGON_ADDVERTEXCOMMAND_H

#include "geodatapolygon_modifyabstractpolygoncommand.h"
#include "../geodatapolygon.h"

#include <QVector2D>

class GeoDataPolygon::AddVertexCommand : public GeoDataPolygon::ModifyAbstractPolygonCommand
{
public:
	AddVertexCommand(bool keyDown, int edgeId, QPoint point, GeoDataPolygon* pol);

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
};

#endif // GEODATAPOLYGON_ADDVERTEXCOMMAND_H
