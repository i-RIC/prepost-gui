#ifndef GEODATAPOLYGON_MOVEVERTEXCOMMAND_H
#define GEODATAPOLYGON_MOVEVERTEXCOMMAND_H

#include "../geodatapolygon.h"

#include <QUndoCommand>
#include <QPolygonF>

class GeoDataPolygon::MoveVertexCommand : public QUndoCommand
{
public:
	MoveVertexCommand(bool keyDown, const QPoint& from, const QPoint& to, vtkIdType vertexId, GeoDataPolygon* pol);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;

	QPolygonF m_newPolygon;
	QPolygonF m_oldPolygon;

	GeoDataPolygon* m_polygon;
	GeoDataPolygonAbstractPolygon* m_targetPolygon;
	bool m_oldMapped;
};

#endif // GEODATAPOLYGON_MOVEVERTEXCOMMAND_H
