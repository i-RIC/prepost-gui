#ifndef GRIDCREATINGCONDITIONTRIANGLEREMESHPOLYGON_H
#define GRIDCREATINGCONDITIONTRIANGLEREMESHPOLYGON_H

#include "gridcreatingconditiontriangleabstractpolygon.h"

class GridCreatingConditionTriangleRemeshPolygonEditGridSizeCommand;

class GridCreatingConditionTriangleRemeshPolygon : public GridCreatingConditionTriangleAbstractPolygon
{
	Q_OBJECT

public:
	GridCreatingConditionTriangleRemeshPolygon(GridCreatingConditionTriangle* parent);
	void setCellSize(double s) {m_cellSize = s;}
	double cellSize() const {return m_cellSize;}
	void finishDefinition() override;

public slots:
	void editGridSize(bool required = false);

private:
	bool m_isCellSizeSet;
	double m_cellSize;

public:
	friend class GridCreatingConditionTriangleRemeshPolygonEditGridSizeCommand;
};

#endif // GRIDCREATINGCONDITIONTRIANGLEREMESHPOLYGON_H
