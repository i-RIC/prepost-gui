#ifndef STRUCTURED2DGRID_H
#define STRUCTURED2DGRID_H

#include "../../guicore_global.h"
#include <vtkStructuredGrid.h>
#include "grid2d.h"
#include <QVector2D>
#include <QLineF>

class QTextStream;

class RectRegion
{
public:
	double xMin;
	double xMax;
	double yMin;
	double yMax;
	RectRegion(double xmin, double xmax, double ymin, double ymax) {
		xMin = xmin;
		xMax = xmax;
		yMin = ymin;
		yMax = ymax;
	}

	bool pointIsInside(double x, double y) const {
		if (x < xMin) {return false;}
		if (x > xMax) {return false;}
		if (y < yMin) {return false;}
		if (y > yMax) {return false;}
		return true;
	}
	bool intersect(const QLineF& line) const {
		bool isInside1 = pointIsInside(line.x1(), line.y1());
		bool isInside2 = pointIsInside(line.x2(), line.y2());
		return (isInside1 != isInside2);
	}
};

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
	/// Constructor
	Structured2DGrid(ProjectDataItem* parent);
	/// Constructor
	Structured2DGrid(const QString& zonename, ProjectDataItem* parent);
	/// Return VTK container object to store the grid.
	vtkStructuredGrid* vtkGrid() {return dynamic_cast<vtkStructuredGrid*>(m_vtkGrid);}
	unsigned int vertexCount() const {return m_dimensionI * m_dimensionJ;}
	unsigned int vertexIndex(unsigned int i, unsigned int j) const {
		return m_dimensionI * j + i;
	}
	void getIJIndex(unsigned int index, unsigned int* i, unsigned int* j);
	unsigned int cellIndex(unsigned int i, unsigned int j) const {
		return (m_dimensionI - 1) * j + i;
	}
	void getCellIJIndex(unsigned int index, unsigned int* i, unsigned int* j);
	const QVector2D vertex(unsigned int index) const;
	const QVector2D vertex(unsigned int i, unsigned int j) const {return vertex(vertexIndex(i, j));}
	void setVertex(unsigned int i, unsigned int j, const QVector2D& v) {setVertex(vertexIndex(i, j), v);}
	void setVertex(unsigned int index, const QVector2D& v);
	bool loadFromCgnsFile(const int fn, int base, int zoneid);
	bool saveToCgnsFile(const int fn, int base, char* zonename);
//	QGraphicsItem* buildGraphicsItem(QGraphicsItem* parent, QGraphicsScene* scene);
	/// getDimension of this grid.
	void dimensions(unsigned int* i, unsigned int* j);
	unsigned int dimensionI() {return m_dimensionI;}
	unsigned int dimensionJ() {return m_dimensionJ;}
	void setDimensions(unsigned int i, unsigned int j);
	unsigned int cellCount() const {
		return m_vtkGrid->GetNumberOfCells();
	}
	const QStringList checkShape(QTextStream& stream);
	bool isValid(QTextStream& stream);
	bool isAspectRatioOk(double limit, QTextStream& stream);
	bool isAngleOk(double limitAngle, QTextStream& stream);
	bool isVariationOk(double ilimit, double jlimit, QTextStream& stream);
	void updateSimplifiedGrid(double xmin, double xmax, double ymin, double ymax);
	int drawnIMin() const {return m_drawnIMin;}
	int drawnIMax() const {return m_drawnIMax;}
	int drawnJMin() const {return m_drawnJMin;}
	int drawnJMax() const {return m_drawnJMax;}
	vtkAlgorithm* vtkFilteredIndexGridAlgorithm() const {return m_vtkFilteredIndexGridAlgorithm;}

private:
	void init();
	int lineLimitI(int j, int iIn, int iOut, const RectRegion& region);
	int lineLimitJ(int i, int jIn, int jOut, const RectRegion& region);
	int lineLimitI2(int iIn, int iOut, const RectRegion& region);
	int lineLimitJ2(int jIn, int jOut, const RectRegion& region);
	bool lineAtIIntersect(int i, const RectRegion& region);
	bool lineAtJIntersect(int j, const RectRegion& region);

	vtkSmartPointer<vtkAlgorithm> m_vtkFilteredIndexGridAlgorithm;

	unsigned int m_drawnIMin;
	unsigned int m_drawnIMax;
	unsigned int m_drawnJMin;
	unsigned int m_drawnJMax;

	unsigned int m_dimensionI;
	unsigned int m_dimensionJ;
};

#endif // STRUCTURED2DGRID_H
