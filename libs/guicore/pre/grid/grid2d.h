#ifndef GRID2D_H
#define GRID2D_H

#include "../../guicore_global.h"
#include "grid.h"
#include <QVector2D>

/// Absract class to store two-dimensional grids.
class GUICOREDLL_EXPORT Grid2D : public Grid
{
public:
	Grid2D(SolverDefinitionGridType::GridType type, ProjectDataItem* parent);
	/// Constructor
	Grid2D(const QString& zonename, SolverDefinitionGridType::GridType type, ProjectDataItem* parent);
	/// Returns the number of vertices in this grid.
	virtual const unsigned int vertexCount() const = 0;
	/// Returns the coordinates of the vertex specified by the index.
	virtual const QVector2D vertex(unsigned int index) const = 0;
	/// Set the coordinates of the vertex specified by the index.
	/**
	 * @param index The index of vertex. It starts from 0, not 1.
	 * @param v The coordinates of the vertex.
	 */
	virtual void setVertex(unsigned int index, const QVector2D& v) = 0;
//	virtual QGraphicsItem* buildGraphicsItem(QGraphicsItem* parent, QGraphicsScene* scene){return 0;}
};

#endif // GRID2D_H
