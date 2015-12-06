#ifndef GRID3D_H
#define GRID3D_H

#include "../../guicore_global.h"
#include "grid.h"

class QVector3D;

class GUICOREDLL_EXPORT Grid3D : public Grid
{

public:
	Grid3D(vtkPointSet* ps, const std::string& zonename, SolverDefinitionGridType::GridType type, ProjectDataItem* parent);
	~Grid3D();
	virtual unsigned int vertexCount() const = 0;
	virtual const QVector3D vertex(unsigned int index) const = 0;
};

#endif // GRID2D_H
