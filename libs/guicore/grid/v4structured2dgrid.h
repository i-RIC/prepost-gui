#ifndef V4STRUCTURED2DGRID_H
#define V4STRUCTURED2DGRID_H

#include "../guicore_global.h"

#include "v4grid2dt.h"

#include <guibase/vtkpointsetextended/vtkstructuredgridextended2d.h>

class vtkPolyData;
class vtkPolyDataExtended2d;

class GUICOREDLL_EXPORT v4Structured2dGrid : public v4Grid2dT<vtkStructuredGridExtended2d>
{
	Q_OBJECT

public:
	v4Structured2dGrid();
	~v4Structured2dGrid();

	void setDimensions(vtkIdType dimI, vtkIdType dimJ);
	void getDimensions(vtkIdType* dimI, vtkIdType* dimJ) const;
	vtkIdType dimensionI() const;
	vtkIdType dimensionJ() const;

	vtkIdType iEdgeCount() const;
	vtkIdType jEdgeCount() const;

	vtkIdType drawnIMin() const;
	vtkIdType drawnIMax() const;
	vtkIdType drawnJMin() const;
	vtkIdType drawnJMax() const;

	vtkIdType pointIndex(vtkIdType i, vtkIdType j) const;
	void getPointIJIndex(vtkIdType index, vtkIdType* i, vtkIdType* j) const;

	vtkIdType cellIndex(vtkIdType i, vtkIdType j) const;
	void getCellIJIndex(vtkIdType index, vtkIdType* i, vtkIdType* j) const;

	vtkIdType iEdgeIndex(vtkIdType i, vtkIdType j) const;
	void getIEdgeIJIndex(vtkIdType index, vtkIdType* i, vtkIdType* j) const;

	vtkIdType jEdgeIndex(vtkIdType i, vtkIdType j) const;
	void getJEdgeIJIndex(vtkIdType index, vtkIdType* i, vtkIdType* j) const;

	QPointF point2d(vtkIdType i, vtkIdType j) const;
	QPointF point2d(vtkIdType index) const;
	void setPoint2d(vtkIdType i, vtkIdType j, const QPointF& v);
	void setPoint2d(vtkIdType index, const QPointF& v);

	vtkPolyDataExtended2d* vtkIEdgeData() const;
	vtkPolyDataExtended2d* vtkJEdgeData() const;

	vtkPolyData* vtkIEdgeFilteredData() const;
	vtkPolyData* vtkJEdgeFilteredData() const;

	vtkPointSet* regionFilteredNodeData(int iMin, int iMax, int jMin, int jMax);
	vtkPointSet* regionFilteredCellData(int iMin, int iMax, int jMin, int jMax);
	vtkPointSet* regionFilteredIEdgeData(int iMin, int iMax, int jMin, int jMax);
	vtkPointSet* regionFilteredJEdgeData(int iMin, int iMax, int jMin, int jMax);

	void updateCellIndex() override;
	void updateFilteredData(double xMin, double xMax, double yMin, double yMax) override;

	v4GridStructureCheckerI* structureChecker() const override;

private:
	class Impl;
	std::unique_ptr<Impl> impl;

	class StructureChecker;
};

#endif // V4STRUCTURED2DGRID_H
