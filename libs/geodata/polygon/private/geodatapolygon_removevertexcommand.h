#ifndef GEODATAPOLYGON_REMOVEVERTEXCOMMAND_H
#define GEODATAPOLYGON_REMOVEVERTEXCOMMAND_H

#include "../geodatapolygon.h"

#include <QUndoCommand>
#include <QVector2D>

class GeoDataPolygon::RemoveVertexCommand : public QUndoCommand
{
public:
	RemoveVertexCommand(vtkIdType vertexId, GeoDataPolygon* pol);

	void redo() override;
	void undo() override;

private:
	vtkIdType m_vertexId;
	QVector2D m_vertexPosition;
	GeoDataPolygon* m_polygon;
	GeoDataPolygonAbstractPolygon* m_targetPolygon;
	bool m_oldMapped;
};

#endif // GEODATAPOLYGON_REMOVEVERTEXCOMMAND_H
