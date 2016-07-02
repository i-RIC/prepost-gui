#ifndef GEODATAPOLYGON_MOVEVERTEXCOMMAND_H
#define GEODATAPOLYGON_MOVEVERTEXCOMMAND_H

#include "../geodatapolygon.h"

#include <QUndoCommand>
#include <QVector2D>

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
	vtkIdType m_vertexId;
	GeoDataPolygon* m_polygon;
	GeoDataPolygonAbstractPolygon* m_targetPolygon;
	QVector2D m_offset;
	bool m_oldMapped;
};

#endif // GEODATAPOLYGON_MOVEVERTEXCOMMAND_H
