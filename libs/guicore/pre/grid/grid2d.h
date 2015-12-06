#ifndef GRID2D_H
#define GRID2D_H

#include "../../guicore_global.h"
#include "grid.h"

class QVector2D;

/// Absract class to store two-dimensional grids.
class GUICOREDLL_EXPORT Grid2D : public Grid
{

public:
	Grid2D(vtkPointSet* ps, SolverDefinitionGridType::GridType type, ProjectDataItem* parent);
	Grid2D(vtkPointSet* ps, const std::string& zonename, SolverDefinitionGridType::GridType type, ProjectDataItem* parent);
	virtual ~Grid2D();

	virtual unsigned int vertexCount() const = 0;
	virtual QVector2D vertex(unsigned int index) const = 0;
	virtual void setVertex(unsigned int index, const QVector2D& v) = 0;
};

#endif // GRID2D_H
