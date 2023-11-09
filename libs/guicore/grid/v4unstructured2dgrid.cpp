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

	bool masked;
	auto filtered = vtkPointSetRegionAndCellSizeFilter::filterGeneral(vtkConcreteData()->concreteData(), region, maxcells, &masked);
	setMasked(masked);

	setFilteredData(filtered);
	int numCells = filtered->GetNumberOfCells();
	filtered->Delete();

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
