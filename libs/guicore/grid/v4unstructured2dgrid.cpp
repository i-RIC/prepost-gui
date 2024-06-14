#include "v4gridutil.h"
#include "v4unstructured2dgrid.h"

#include <guibase/vtktool/vtkpointsetregionandcellsizefilter.h>
#include <misc/rectregion.h>
#include <misc/stringtool.h>

v4Unstructured2dGrid::v4Unstructured2dGrid() :
	v4Grid2dT<vtkUnstructuredGridExtended2d> {}
{}

void v4Unstructured2dGrid::updateFilteredData(double xMin, double xMax, double yMin, double yMax)
{
	bool cullEnable;
	int cullCellLimit, cullIndexLimit;
	v4GridUtil::getCullSetting(&cullEnable, &cullCellLimit, &cullIndexLimit);

	int maxcells = -1;
	if (cullEnable) {
		maxcells = cullCellLimit;
	}
	RectRegion region(xMin, xMax, yMin, yMax);

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

	bool masked;
	auto filtered = vtkPointSetRegionAndCellSizeFilter::filterGeneral(concreteData, region, maxcells, &masked);
	setMasked(masked);

	setFilteredData(filtered);
	int numCells = filtered->GetNumberOfCells();
	filtered->Delete();

	concreteData->GetPointData()->RemoveArray(ORIGINAL_ID);
	concreteData->GetCellData()->RemoveArray(ORIGINAL_ID);

	vtkPointSet* indexData = nullptr;
	if (! cullEnable && numCells < cullIndexLimit) {
		indexData = filtered;
		indexData->Register(nullptr);
	} else {
		auto mask = vtkSmartPointer<vtkMaskPolyData>::New();
		int ratio = static_cast<int>(numCells / cullIndexLimit);
		if (ratio == 1) {ratio = 2;}
		mask->SetOnRatio(ratio);
		mask->SetInputData(filtered);
		mask->Update();
		indexData = mask->GetOutput();
		indexData->Register(nullptr);
	}

	auto filteredIndexGrid = vtkSmartPointer<vtkPolyData>::New();
	auto igPoints = vtkSmartPointer<vtkPoints>::New();
	auto ca = vtkSmartPointer<vtkCellArray>::New();
	auto sa = vtkSmartPointer<vtkStringArray>::New();
	sa->SetName(v4GridUtil::LABEL_NAME);
	vtkIdType cellid = 0;
	double tmpp[3];
	QString label("(%1)");

	auto grid = vtkConcreteData()->concreteData();
	std::unordered_set<vtkIdType> indices;

	for (int i = 0; i < indexData->GetNumberOfCells(); ++i) {
		vtkCell* cell = indexData->GetCell(i);
		for (int j = 0; j < cell->GetNumberOfPoints(); ++j) {
			vtkIdType vid = cell->GetPointId(j);
			if (indices.find(vid) == indices.end()) {
				grid->GetPoint(vid, tmpp);
				igPoints->InsertNextPoint(tmpp);
				ca->InsertNextCell(1, &cellid);
				sa->InsertNextValue(iRIC::toStr(label.arg(vid + 1)));
				++ cellid;
				indices.insert(vid);
			}
		}
	}
	filteredIndexGrid->SetPoints(igPoints);
	filteredIndexGrid->SetVerts(ca);
	filteredIndexGrid->GetPointData()->AddArray(sa);

	setFilteredIndexData(filteredIndexGrid);
	indexData->Delete();
}
