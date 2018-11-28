#ifndef GEODATAPOLYLINE_ADDVERTEXCOMMAND_H
#define GEODATAPOLYLINE_ADDVERTEXCOMMAND_H

#include "geodatapolyline_modifyabstractpolylinecommand.h"
#include "../geodatapolyline.h"

#include <QVector2D>

class GeoDataPolyLine::AddVertexCommand : public GeoDataPolyLine::ModifyAbstractPolyLineCommand
{
public:
	AddVertexCommand(bool keyDown, int edgeId, QPoint point, GeoDataPolyLine* pol);

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
};

#endif // GEODATAPOLYLINE_ADDVERTEXCOMMAND_H
