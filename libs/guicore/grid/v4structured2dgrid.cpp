#include "v4gridutil.h"
#include "v4structured2dgrid.h"
#include "private/v4structured2dgrid_impl.h"

#include <guibase/vtktool/vtkpointsetregionandcellsizefilter.h>
#include <misc/rectregion.h>
#include <misc/stringtool.h>

#include <vtkExtractCells.h>
#include <vtkExtractGrid.h>
#include <vtkIdList.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

v4Structured2dGrid::v4Structured2dGrid() :
	v4Grid2dT<vtkStructuredGridExtended2d> {},
	impl {new Impl {this}}
{}

v4Structured2dGrid::~v4Structured2dGrid()
{}

void v4Structured2dGrid::clear()
{
	v4Grid::clear();

	impl->m_vtkIEdgeData.data()->GetCellData()->Initialize();
	impl->m_vtkJEdgeData.data()->GetCellData()->Initialize();
}

void v4Structured2dGrid::updateValueRangeSet()
{
	v4Grid::updateValueRangeSet();

	impl->m_vtkIEdgeData.updateValueRangeSet();
	impl->m_vtkJEdgeData.updateValueRangeSet();
}

void v4Structured2dGrid::setDimensions(vtkIdType dimI, vtkIdType dimJ)
{
	impl->m_dimensionI = dimI;
	impl->m_dimensionJ = dimJ;

	impl->setupVtkObjects();
}

void v4Structured2dGrid::getDimensions(vtkIdType* dimI, vtkIdType* dimJ) const
{
	*dimI = impl->m_dimensionI;
	*dimJ = impl->m_dimensionJ;
}

vtkIdType v4Structured2dGrid::dimensionI() const
{
	return impl->m_dimensionI;
}

vtkIdType v4Structured2dGrid::dimensionJ() const
{
	return impl->m_dimensionJ;
}

vtkIdType v4Structured2dGrid::iEdgeCount() const
{
	return impl->m_dimensionI * (impl->m_dimensionJ - 1);
}

vtkIdType v4Structured2dGrid::jEdgeCount() const
{
	return (impl->m_dimensionI - 1) * impl->m_dimensionJ;
}

vtkIdType v4Structured2dGrid::drawnIMin() const
{
	return impl->m_drawnIMin;
}

vtkIdType v4Structured2dGrid::drawnIMax() const
{
	return impl->m_drawnIMax;
}

vtkIdType v4Structured2dGrid::drawnJMin() const
{
	return impl->m_drawnJMin;
}

vtkIdType v4Structured2dGrid::drawnJMax() const
{
	return impl->m_drawnJMax;
}

vtkIdType v4Structured2dGrid::pointIndex(vtkIdType i, vtkIdType j) const
{
	return impl->m_dimensionI * j + i;
}

void v4Structured2dGrid::getPointIJIndex(vtkIdType index, vtkIdType* i, vtkIdType* j) const
{
	*i = index % impl->m_dimensionI;
	*j = index / impl->m_dimensionI;
}

vtkIdType v4Structured2dGrid::cellIndex(vtkIdType i, vtkIdType j) const
{
	return (impl->m_dimensionI - 1) * j + i;
}

void v4Structured2dGrid::getCellIJIndex(vtkIdType index, vtkIdType* i, vtkIdType* j) const
{
	*i = index % (impl->m_dimensionI - 1);
	*j = index / (impl->m_dimensionI - 1);
}

vtkIdType v4Structured2dGrid::iEdgeIndex(vtkIdType i, vtkIdType j) const
{
	return impl->m_dimensionI * j + i;
}

void v4Structured2dGrid::getIEdgeIJIndex(vtkIdType index, vtkIdType* i, vtkIdType* j) const
{
	*i = index % impl->m_dimensionI;
	*j = index / impl->m_dimensionI;
}

vtkIdType v4Structured2dGrid::jEdgeIndex(vtkIdType i, vtkIdType j) const
{
	return (impl->m_dimensionI - 1) * j + i;
}

void v4Structured2dGrid::getJEdgeIJIndex(vtkIdType index, vtkIdType* i, vtkIdType* j) const
{
	*i = index % (impl->m_dimensionI - 1);
	*j = index / (impl->m_dimensionI - 1);
}

QPointF v4Structured2dGrid::point2d(vtkIdType i, vtkIdType j) const
{
	return v4Grid2d::point2d(pointIndex(i, j));
}

QPointF v4Structured2dGrid::point2d(vtkIdType index) const
{
	return v4Grid2d::point2d(index);
}

void v4Structured2dGrid::setPoint2d(vtkIdType i, vtkIdType j, const QPointF& v)
{
	v4Grid2d::setPoint2d(pointIndex(i, j), v);
}

void v4Structured2dGrid::setPoint2d(vtkIdType index, const QPointF& v)
{
	v4Grid2d::setPoint2d(index, v);
}

vtkPolyDataExtended2d* v4Structured2dGrid::vtkIEdgeData() const
{
	return &impl->m_vtkIEdgeData;
}

vtkPolyDataExtended2d* v4Structured2dGrid::vtkJEdgeData() const
{
	return &impl->m_vtkJEdgeData;
}

vtkPolyData* v4Structured2dGrid::vtkIEdgeFilteredData() const
{
	return impl->m_vtkIEdgeFilteredData;
}

vtkPolyData* v4Structured2dGrid::vtkJEdgeFilteredData() const
{
	return impl->m_vtkJEdgeFilteredData;
}

vtkPointSet* v4Structured2dGrid::regionFilteredNodeData(int iMin, int iMax, int jMin, int jMax)
{
	auto extract = vtkSmartPointer<vtkExtractGrid>::New();
	extract->SetInputData(vtkConcreteData()->data());
	extract->SetVOI(iMin, iMax, jMin, jMax, 0, 0);
	extract->Update();
	auto ret = extract->GetOutput();

	ret->Register(nullptr);
	return ret;
}

vtkPointSet* v4Structured2dGrid::regionFilteredCellData(int iMin, int iMax, int jMin, int jMax)
{
	auto extract = vtkSmartPointer<vtkExtractGrid>::New();
	extract->SetInputData(vtkConcreteData()->data());
	extract->SetVOI(iMin, iMax + 1, jMin, jMax + 1, 0, 0);
	extract->Update();
	auto ret = extract->GetOutput();

	ret->Register(nullptr);
	return ret;
}

vtkPointSet* v4Structured2dGrid::regionFilteredIEdgeData(int iMin, int iMax, int jMin, int jMax)
{
	auto cellList = vtkSmartPointer<vtkIdList>::New();
	for (int j = jMin; j <= jMax; ++j) {
		for (int i = iMin; i <= iMax; ++i) {
			cellList->InsertNextId(iEdgeIndex(i, j));
		}
	}
	auto extract = vtkSmartPointer<vtkExtractCells>::New();
	extract->SetInputData(vtkIEdgeData()->data());
	extract->SetCellList(cellList);
	extract->Update();
	auto ret = extract->GetOutput();

	ret->Register(nullptr);
	return ret;
}

vtkPointSet* v4Structured2dGrid::regionFilteredJEdgeData(int iMin, int iMax, int jMin, int jMax)
{
	auto cellList = vtkSmartPointer<vtkIdList>::New();
	for (int j = jMin; j <= jMax; ++j) {
		for (int i = iMin; i <= iMax; ++i) {
			cellList->InsertNextId(jEdgeIndex(i, j));
		}
	}
	auto extract = vtkSmartPointer<vtkExtractCells>::New();
	extract->SetInputData(vtkJEdgeData()->data());
	extract->SetCellList(cellList);
	extract->Update();
	auto ret = extract->GetOutput();

	ret->Register(nullptr);
	return ret;
}

vtkPolyData* v4Structured2dGrid::extractEdgeData(vtkIdType i, vtkIdType j) const
{
	auto ret = vtkPolyData::New();
	ret->SetPoints(vtkConcreteData()->data()->GetPoints());

	auto ca = vtkSmartPointer<vtkCellArray>::New();
	std::vector<vtkIdType> ids;
	if (i < 0) {
		ids.reserve(dimensionI());
		for (vtkIdType ii = 0; ii < dimensionI(); ++ii) {
			vtkIdType index = pointIndex(ii, j);
			ids.push_back(index);
		}
		ca->InsertNextCell(ids.size(), ids.data());
	} else if (j < 0) {
		ids.reserve(dimensionJ());
		for (vtkIdType jj = 0; jj < dimensionJ(); ++jj) {
			vtkIdType index = pointIndex(i, jj);
			ids.push_back(index);
		}
		ca->InsertNextCell(ids.size(), ids.data());
	}
	ret->SetLines(ca);

	return ret;
}

vtkStructuredGrid* v4Structured2dGrid::cellCenterGrid() const
{
	return impl->m_cellCenterGrid;
}

vtkStructuredGrid* v4Structured2dGrid::iEdgeCenterGrid() const
{
	return impl->m_iEdgeCenterGrid;
}

vtkStructuredGrid* v4Structured2dGrid::jEdgeCenterGrid() const
{
	return impl->m_jEdgeCenterGrid;
}

void v4Structured2dGrid::updateCenterGridData()
{
	int dim[3];
	vtkConcreteData()->concreteData()->GetDimensions(dim);

	auto points = vtkConcreteData()->concreteData()->GetPoints();

	// cell center
	impl->m_cellCenterGrid->SetDimensions(dim[0] - 1, dim[1] - 1, 1);
	auto cellPoints = vtkSmartPointer<vtkPoints>::New();
	cellPoints->SetDataTypeToDouble();
	cellPoints->Allocate((dim[0] - 1) * (dim[1] - 1));
	for (int j = 0; j < dim[1] - 1; ++j) {
		for (int i = 0; i < dim[0] - 1; ++i) {
			double p00[3], p10[3], p01[3], p11[3];
			points->GetPoint(pointIndex(i,     j    ), p00);
			points->GetPoint(pointIndex(i + 1, j    ), p10);
			points->GetPoint(pointIndex(i,     j + 1), p01);
			points->GetPoint(pointIndex(i + 1, j + 1), p11);
			cellPoints->InsertNextPoint(
				(p00[0] + p10[0] + p01[0] + p11[0]) / 4.0,
				(p00[1] + p10[1] + p01[1] + p11[1]) / 4.0,
				0.0);
		}
	}
	impl->m_cellCenterGrid->SetPoints(cellPoints);
	impl->m_cellCenterGrid->GetPointData()->Initialize();

	auto cellData = vtkConcreteData()->concreteData()->GetCellData();
	for (int i = 0; i < cellData->GetNumberOfArrays(); ++i) {
		auto array = cellData->GetArray(i);
		impl->m_cellCenterGrid->GetPointData()->AddArray(array);
	}

	// i-edge center
	impl->m_iEdgeCenterGrid->SetDimensions(dim[0], dim[1] - 1, 1);
	auto iEdgePoints = vtkSmartPointer<vtkPoints>::New();
	iEdgePoints->SetDataTypeToDouble();
	iEdgePoints->Allocate(dim[0] * (dim[1] - 1));
	for (int j = 0; j < dim[1] - 1; ++j) {
		for (int i = 0; i < dim[0]; ++i) {
			double p0[3], p1[3];
			points->GetPoint(pointIndex(i, j    ), p0);
			points->GetPoint(pointIndex(i, j + 1), p1);
			iEdgePoints->InsertNextPoint(
				(p0[0] + p1[0]) / 2.0,
				(p0[1] + p1[1]) / 2.0,
				0.0);
		}
	}
	impl->m_iEdgeCenterGrid->SetPoints(iEdgePoints);
	impl->m_iEdgeCenterGrid->GetPointData()->Initialize();

	cellData = vtkIEdgeData()->concreteData()->GetCellData();
	for (int i = 0; i < cellData->GetNumberOfArrays(); ++i) {
		auto array = cellData->GetArray(i);
		impl->m_iEdgeCenterGrid->GetPointData()->AddArray(array);
	}

	// j-edge center
	impl->m_jEdgeCenterGrid->SetDimensions(dim[0] - 1, dim[1], 1);
	auto jEdgePoints = vtkSmartPointer<vtkPoints>::New();
	jEdgePoints->SetDataTypeToDouble();
	jEdgePoints->Allocate((dim[0] - 1) * dim[1]);
	for (int j = 0; j < dim[1]; ++j) {
		for (int i = 0; i < dim[0] - 1; ++i) {
			double p0[3], p1[3];
			points->GetPoint(pointIndex(i,     j), p0);
			points->GetPoint(pointIndex(i + 1, j), p1);
			jEdgePoints->InsertNextPoint(
				(p0[0] + p1[0]) / 2.0,
				(p0[1] + p1[1]) / 2.0,
				0.0);
		}
	}
	impl->m_jEdgeCenterGrid->SetPoints(jEdgePoints);
	impl->m_jEdgeCenterGrid->GetPointData()->Initialize();

	cellData = vtkJEdgeData()->concreteData()->GetCellData();
	for (int i = 0; i < cellData->GetNumberOfArrays(); ++i) {
		auto array = cellData->GetArray(i);
		impl->m_jEdgeCenterGrid->GetPointData()->AddArray(array);
	}
}

void v4Structured2dGrid::discardCellIndex()
{
	v4Grid::discardCellIndex();
	impl->m_vtkIEdgeData.discardCellIndex();
	impl->m_vtkJEdgeData.discardCellIndex();
}

void v4Structured2dGrid::updateFilteredData(double xMin, double xMax, double yMin, double yMax)
{
	setMasked(false);

	bool outOfRegion;
	int iMin, iMax, jMin, jMax;
	int rate;

	bool cullEnable;
	int cullCellLimit, cullIndexLimit;

	v4GridUtil::getCullSetting(&cullEnable, &cullCellLimit, &cullIndexLimit);

	RectRegion region(xMin, xMax, yMin, yMax);

	int maxcell = -1;
	if (cullEnable) {
		maxcell = cullCellLimit;
	}
	int indicesRate = 1;

	vtkPointSetRegionAndCellSizeFilter::calcExtractParameters(vtkConcreteData()->concreteData(), region, vtkData()->pointLocator(), maxcell, &outOfRegion, &iMin, &iMax, &jMin, &jMax, &rate);
	int indicesCount = (jMax - jMin + 1) + (iMax - iMin + 1);
	if (indicesCount > cullIndexLimit) {
		indicesRate = indicesCount / cullIndexLimit;
	}

	if (outOfRegion) {
		auto emptyPolyData = vtkSmartPointer<vtkPolyData>::New();
		setFilteredData(emptyPolyData);
		setFilteredIndexData(emptyPolyData);
		auto emptyCells = vtkSmartPointer<vtkCellArray>::New();
		impl->m_vtkIEdgeFilteredData->SetLines(emptyCells);
		impl->m_vtkJEdgeFilteredData->SetLines(emptyCells);

		impl->m_drawnIMin = -1;
		impl->m_drawnIMax = -2;
		impl->m_drawnJMin = -1;
		impl->m_drawnJMax = -2;

		return;
	}

	auto concreteData = vtkConcreteData()->concreteData();

	auto origPointIds = vtkSmartPointer<vtkIntArray>::New();
	origPointIds->SetName(ORIGINAL_ID);
	origPointIds->Allocate(concreteData->GetNumberOfPoints());
	for (vtkIdType index = 0; index < concreteData->GetNumberOfPoints(); ++index) {
		origPointIds->InsertNextValue(index);
	}

	auto origCellIds = vtkSmartPointer<vtkIntArray>::New();
	origCellIds->SetName(ORIGINAL_ID);
	origCellIds->Allocate(concreteData->GetNumberOfCells());
	for (vtkIdType index = 0; index < concreteData->GetNumberOfCells(); ++index) {
		origCellIds->InsertNextValue(index);
	}

	concreteData->GetPointData()->AddArray(origPointIds);
	concreteData->GetCellData()->AddArray(origCellIds);

	auto filteredGrid = vtkPointSetRegionAndCellSizeFilter::extract(concreteData, iMin, iMax, jMin, jMax, rate);
	setFilteredData(filteredGrid);
	filteredGrid->Delete();

	concreteData->GetPointData()->RemoveArray(ORIGINAL_ID);
	concreteData->GetCellData()->RemoveArray(ORIGINAL_ID);

	impl->m_drawnIMin = iMin;
	impl->m_drawnIMax = iMax;
	impl->m_drawnJMin = jMin;
	impl->m_drawnJMax = jMax;

	setMasked(rate > 1);

	int tmpIMin = (impl->m_drawnIMin / rate) * rate;
	int tmpIMax = std::min((impl->m_drawnIMax / rate + 1) * rate, impl->m_dimensionI - 1);
	int tmpJMin = (impl->m_drawnJMin / rate) * rate;
	int tmpJMax = std::min((impl->m_drawnJMax / rate + 1) * rate, impl->m_dimensionJ - 1);

	auto filteredIndexGrid = vtkSmartPointer<vtkPolyData>::New();
	auto points = vtkSmartPointer<vtkPoints>::New();
	points->SetDataTypeToDouble();
	auto ca = vtkSmartPointer<vtkCellArray>::New();
	auto sa = vtkSmartPointer<vtkStringArray>::New();
	sa->SetName(v4GridUtil::LABEL_NAME);

	QString label("(%1,%2)");
	auto grid = vtkConcreteData()->concreteData();
	filteredIndexGrid->SetPoints(points);
	double p[3];
	vtkIdType pointId = 0;
	for (int i = tmpIMin; i <= tmpIMax; i += indicesRate) {
		grid->GetPoint(pointIndex(i, 0), p);
		points->InsertNextPoint(p);
		ca->InsertNextCell(1, &pointId);
		sa->InsertNextValue(iRIC::toStr(label.arg(i + 1).arg(1)));
		++ pointId;
	}
	for (int j = tmpJMin; j <= tmpJMax; j += indicesRate) {
		grid->GetPoint(pointIndex(0, j), p);
		points->InsertNextPoint(p);
		ca->InsertNextCell(1, &pointId);
		sa->InsertNextValue(iRIC::toStr(label.arg(1).arg(j + 1)));
		++ pointId;
	}

	filteredIndexGrid->SetVerts(ca);
	filteredIndexGrid->GetPointData()->AddArray(sa);

	setFilteredIndexData(filteredIndexGrid);

	auto srcIEdgeCellData = vtkIEdgeData()->data()->GetCellData();
	auto tgtIEdgeCellData = impl->m_vtkIEdgeFilteredData->GetCellData();
	tgtIEdgeCellData->Initialize();
	tgtIEdgeCellData->CopyAllocate(srcIEdgeCellData);
	vtkIdType pointIds[2];
	auto iEdgeCells = vtkSmartPointer<vtkCellArray>::New();
	int iEdgeTargetId = 0;
	for (int j = jMin; j < jMax && j + rate < impl->m_dimensionJ; j += rate) {
		for (int i = iMin; i <= iMax; i += rate) {
			pointIds[0] = pointIndex(i, j);
			pointIds[1] = pointIndex(i, j + rate);
			iEdgeCells->InsertNextCell(2, pointIds);
			auto edgeId = iEdgeIndex(i, j);
			tgtIEdgeCellData->CopyData(srcIEdgeCellData, edgeId, iEdgeTargetId++);
		}
	}
	impl->m_vtkIEdgeFilteredData->SetLines(iEdgeCells);

	auto srcJEdgeCellData = vtkJEdgeData()->data()->GetCellData();
	auto tgtJEdgeCellData = impl->m_vtkJEdgeFilteredData->GetCellData();
	tgtJEdgeCellData->Initialize();
	tgtJEdgeCellData->CopyAllocate(srcJEdgeCellData);
	auto jEdgeCells = vtkSmartPointer<vtkCellArray>::New();
	int jEdgeTargetId = 0;
	for (int i = iMin; i < iMax && i + rate < impl->m_dimensionI; i += rate) {
		for (int j = jMin; j <= jMax; j += rate) {
			pointIds[0] = pointIndex(i, j);
			pointIds[1] = pointIndex(i + rate, j);
			jEdgeCells->InsertNextCell(2, pointIds);
			auto edgeId = jEdgeIndex(i, j);
			tgtJEdgeCellData->CopyData(srcJEdgeCellData, edgeId, jEdgeTargetId++);
		}
	}
	impl->m_vtkJEdgeFilteredData->SetLines(jEdgeCells);
}

v4GridStructureCheckerI* v4Structured2dGrid::structureChecker() const
{
	return impl->m_structureChecker;
}
