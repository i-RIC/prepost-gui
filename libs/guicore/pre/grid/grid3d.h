#ifndef GRID3D_H
#define GRID3D_H

#include "../../guicore_global.h"
#include "grid.h"
#include <QVector3D>

class GUICOREDLL_EXPORT Grid3D : public Grid
{
public:
	/// Constructor
	Grid3D(const QString& zonename, SolverDefinitionGridType::GridType type, ProjectDataItem* parent);
	virtual unsigned int vertexCount() const = 0;
	virtual const QVector3D vertex(unsigned int index) const = 0;
	virtual void load() = 0;
	virtual void save() = 0;
};

#endif // GRID2D_H
