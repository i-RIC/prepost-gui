#ifndef UNSTRUCTURED2DGRID_H
#define UNSTRUCTURED2DGRID_H

#include "../../guicore_global.h"
#include "grid2d.h"
#include <vtkUnstructuredGrid.h>
#include <vtkSmartPointer.h>

/// Unstructured two-dimensional grid
class GUICOREDLL_EXPORT Unstructured2DGrid : public Grid2D
{
	Q_OBJECT

public:
	Unstructured2DGrid(ProjectDataItem* parent);
	Unstructured2DGrid(const std::string& zonename, ProjectDataItem* parent);
	virtual ~Unstructured2DGrid();

	vtkUnstructuredGrid* vtkGrid() const;
	unsigned int vertexCount() const override;
	unsigned int cellCount() const override;

	QPointF vertex(unsigned int index) const override;
	void setVertex(unsigned int index, const QPointF& v) override;

	bool loadFromCgnsFile(const int fn, int B, int Z) override;
	bool saveToCgnsFile(const int fn, int B, const char* zonename) override;

	void updateSimplifiedGrid(double xmin, double xmax, double ymin, double ymax) override;

	vtkAlgorithm* vtkFilteredIndexGridAlgorithm() const override;

private:
	void setupIndexArray();

	vtkSmartPointer<vtkAlgorithm> m_vtkFilteredIndexGridAlgorithm;
};

#endif // UNSTRUCTURED2DGRID_H
