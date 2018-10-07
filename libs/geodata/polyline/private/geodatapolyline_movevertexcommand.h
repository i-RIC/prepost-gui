#ifndef GEODATAPOLYLINE_MOVEVERTEXCOMMAND_H
#define GEODATAPOLYLINE_MOVEVERTEXCOMMAND_H

#include "../geodatapolyline.h"
#include "geodatapolyline_modifyabstractpolylinecommand.h"

class GeoDataPolyLine::MoveVertexCommand : public GeoDataPolyLine::ModifyAbstractPolyLineCommand
{
public:
	MoveVertexCommand(bool keyDown, const QPoint& from, const QPoint& to, int vertexId, GeoDataPolyLine* pol);

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
};

#endif // GEODATAPOLYLINE_MOVEVERTEXCOMMAND_H
