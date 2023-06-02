#ifndef STRUCTURED2DGRID_H
#define STRUCTURED2DGRID_H

#include "../../guicore_global.h"
#include "grid2d.h"

#include <vtkAlgorithm.h>
#include <vtkSmartPointer.h>
#include <vtkStructuredGrid.h>

class RectRegion;

class QTextStream;
class QPointF;

/// Structured two-dimensional grid
/**
 * This class is used to store all the following type grids:
 * - Cartesian grid
 * - Orthogonal grid
 * - Boundary-fitted coordinates grid
 */
class GUICOREDLL_EXPORT Structured2DGrid : public Grid2D
{
	Q_OBJECT

public:
	Structured2DGrid(ProjectDataItem* parent);
	Structured2DGrid(const std::string& zonename, ProjectDataItem* parent);
	~Structured2DGrid();

	vtkStructuredGrid* vtkGrid() const;
	unsigned int vertexCount() const override;
	unsigned int cellCount() const override;

	unsigned int vertexIndex(unsigned int i, unsigned int j) const;
	void getIJIndex(unsigned int index, unsigned int* i, unsigned int* j);

	unsigned int cellIndex(unsigned int i, unsigned int j) const;
	void getCellIJIndex(unsigned int index, unsigned int* i, unsigned int* j);

	QPointF vertex(unsigned int index) const override;
	QPointF vertex(unsigned int i, unsigned int j) const;

	void setVertex(unsigned int i, unsigned int j, const QPointF& v);
	void setVertex(unsigned int index, const QPointF& v) override;

	int loadFromCgnsFile(const iRICLib::H5CgnsZone& zone) override;
	int saveToCgnsFile(iRICLib::H5CgnsBase* base, const std::string& zoneName) override;

	void dimensions(unsigned int* i, unsigned int* j);
	unsigned int dimensionI() const;
	unsigned int dimensionJ() const;
	void setDimensions(unsigned int i, unsigned int j);
	void setDimensionsFromVtkGrid();

	const QStringList checkShape(QTextStream& stream) override;
	bool isValid(QTextStream& stream) const override;
	bool isAspectRatioOk(double limit, QTextStream& stream);
	bool isAngleOk(double limitAngle, QTextStream& stream);
	bool isVariationOk(double ilimit, double jlimit, QTextStream& stream);

	void updateSimplifiedGrid(double xmin, double xmax, double ymin, double ymax) override;

	int drawnIMin() const;
	int drawnIMax() const;
	int drawnJMin() const;
	int drawnJMax() const;

private:
	unsigned int m_drawnIMin;
	unsigned int m_drawnIMax;
	unsigned int m_drawnJMin;
	unsigned int m_drawnJMax;

	unsigned int m_dimensionI;
	unsigned int m_dimensionJ;
};

#endif // STRUCTURED2DGRID_H
