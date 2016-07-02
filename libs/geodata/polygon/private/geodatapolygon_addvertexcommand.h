#ifndef GEODATAPOLYGON_ADDVERTEXCOMMAND_H
#define GEODATAPOLYGON_ADDVERTEXCOMMAND_H

#include "../geodatapolygon.h"

#include <QUndoCommand>
#include <QVector2D>

class GeoDataPolygon::AddVertexCommand : public QUndoCommand
{
public:
	AddVertexCommand(bool keyDown, vtkIdType edgeId, QPoint point, GeoDataPolygon* pol);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	bool m_keyDown;
	vtkIdType m_vertexId;
	QVector2D m_vertexPosition;
	GeoDataPolygon* m_polygon;
	GeoDataPolygonAbstractPolygon* m_targetPolygon;
	bool m_oldMapped;
};

#endif // GEODATAPOLYGON_ADDVERTEXCOMMAND_H
