#ifndef UNSTRUCTURED2DGRID_H
#define UNSTRUCTURED2DGRID_H

#include "../../guicore_global.h"
#include "grid2d.h"
#include <vtkUnstructuredGrid.h>
#include <vtkSmartPointer.h>

/// Unstructured two-dimensional grid
class GUICOREDLL_EXPORT Unstructured2DGrid : public Grid2D {
	Q_OBJECT
public:
	/// Constructor
	Unstructured2DGrid(ProjectDataItem* parent);
	/// Constructor
	Unstructured2DGrid(const QString& zonename, ProjectDataItem* parent);
	virtual ~Unstructured2DGrid(){}
	/// Return VTK container object to store the grid.
	vtkUnstructuredGrid* vtkGrid(){return dynamic_cast<vtkUnstructuredGrid*>(m_vtkGrid);}
	unsigned int vertexCount() const;
	const QVector2D vertex(unsigned int index) const;
	void setVertex(unsigned int index, const QVector2D& v);
	bool loadFromCgnsFile(const int fn, int B, int Z);
	bool saveToCgnsFile(const int fn, int B, char* zonename);
	unsigned int cellCount() const {
		return m_vtkGrid->GetNumberOfCells();
	}
	vtkAlgorithm* vtkFilteredIndexGridAlgorithm() const {return m_vtkFilteredIndexGridAlgorithm;}
	void updateSimplifiedGrid(double xmin, double xmax, double ymin, double ymax);
private:
	void init();
	void setupIndexArray();

	vtkSmartPointer<vtkAlgorithm> m_vtkFilteredIndexGridAlgorithm;
};

#endif // UNSTRUCTURED2DGRID_H
