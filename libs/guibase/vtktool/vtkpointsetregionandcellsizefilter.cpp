#include "vtkpointsetregionandcellsizefilter.h"

#include <misc/rectregion.h>

#include <vtkAbstractPointLocator.h>
#include <vtkExtractGrid.h>
#include <vtkGeometryFilter.h>
#include <vtkMaskPolyData.h>
#include <vtkSmartPointer.h>

#include <cmath>

namespace {

const double MARGIN_RATIO = 0.2;

bool pointIsInsideRegion(const RectRegion& region, vtkStructuredGrid* grid, int* dimensions, int i, int j)
{
	double v[3];
	vtkIdType index = *(dimensions) * j + i;
	grid->GetPoint(index, v);

	return region.pointIsInside(v[0], v[1]);
}

bool iLineIntersectsRegion(const RectRegion& region, vtkStructuredGrid* grid, int* dimensions, int i)
{
	double v[3];
	for (int j = 0; j < *(dimensions + 1); ++j) {
		vtkIdType index = *(dimensions) * j + i;
		grid->GetPoint(index, v);
		if (region.pointIsInside(v[0], v[1])) {return true;}
	}

	return false;
}

bool jLineIntersectsRegion(const RectRegion& region, vtkStructuredGrid* grid, int* dimensions, int j)
{
	double v[3];
	for (int i = 0; i < *(dimensions); ++i) {
		vtkIdType index = *(dimensions) * j + i;
		grid->GetPoint(index, v);
		if (region.pointIsInside(v[0], v[1])) {return true;}
	}

	return false;
}

int limitIIntersects(const RectRegion& region, vtkStructuredGrid* grid, int* dimensions, int iIn, int iOut, int j)
{
	if (std::abs(iOut - iIn) == 1) {
		return iIn;
	}

	int i = (iIn + iOut) / 2;
	if (pointIsInsideRegion(region, grid, dimensions, i, j)) {
		return limitIIntersects(region, grid, dimensions, i, iOut, j);
	} else {
		return limitIIntersects(region, grid, dimensions, iIn, i, j);
	}
}

int limitIIntersects(const RectRegion& region, vtkStructuredGrid* grid, int* dimensions, int iIn, int iOut)
{
	if (std::abs(iOut - iIn) == 1) {
		return iIn;
	}

	int i = (iIn + iOut) / 2;
	if (iLineIntersectsRegion(region, grid, dimensions, i)) {
		return limitIIntersects(region, grid, dimensions, i, iOut);
	} else {
		return limitIIntersects(region, grid, dimensions, iIn, i);
	}
}

int limitJIntersects(const RectRegion& region, vtkStructuredGrid* grid, int* dimensions, int jIn, int jOut, int i)
{
	if (std::abs(jOut - jIn) == 1) {
		return jIn;
	}

	int j = (jIn + jOut) / 2;
	if (pointIsInsideRegion(region, grid, dimensions, i, j)) {
		return limitJIntersects(region, grid, dimensions, j, jOut, i);
	} else {
		return limitJIntersects(region, grid, dimensions, jIn, j, i);
	}
}

int limitJIntersects(const RectRegion& region, vtkStructuredGrid* grid, int* dimensions, int jIn, int jOut)
{
	if (std::abs(jOut - jIn) == 1) {
		return jIn;
	}

	int j = (jIn + jOut) / 2;
	if (jLineIntersectsRegion(region, grid, dimensions, j)) {
		return limitJIntersects(region, grid, dimensions, j, jOut);
	} else {
		return limitJIntersects(region, grid, dimensions, jIn, j);
	}
}


} // namespace

vtkPolyData* vtkPointSetRegionAndCellSizeFilter::filterGeneral(vtkPointSet* data, const RectRegion& region, int maxcell, bool* masked)
{
	*masked = false;

	double xwidth = region.xMax - region.xMin;
	double ywidth = region.yMax - region.yMin;

	auto gfilter = vtkSmartPointer<vtkGeometryFilter>::New();
	gfilter->SetExtent(region.xMin - xwidth * MARGIN_RATIO, region.xMax + xwidth * MARGIN_RATIO, region.yMin - ywidth * MARGIN_RATIO, region.yMax + ywidth * MARGIN_RATIO, -1, 1);
	gfilter->ExtentClippingOn();
	gfilter->SetInputData(data);
	gfilter->Update();

	auto clipped = gfilter->GetOutput();
	auto numCells = clipped->GetNumberOfCells();

	if (maxcell < 0 || numCells <= maxcell) {
		clipped->Register(nullptr);
		return clipped;
	}

	auto mask = vtkSmartPointer<vtkMaskPolyData>::New();
	auto ratio = static_cast<int> (numCells / maxcell);
	if (ratio == 1) {ratio = 2;}
	mask->SetOnRatio(ratio);
	mask->SetInputData(clipped);
	mask->Update();

	auto maskedData = mask->GetOutput();
	maskedData->Register(nullptr);
	*masked = true;
	return maskedData;
}

void vtkPointSetRegionAndCellSizeFilter::calcExtractParameters(vtkStructuredGrid* data, const RectRegion& region, vtkAbstractPointLocator* pointLocator, int maxcell, bool* outOfRegion, int* imin, int* imax, int* jmin, int* jmax, int* rate)
{
	*outOfRegion = false;

	double xmin = region.xMin;
	double xmax = region.xMax;
	double ymin = region.yMin;
	double ymax = region.yMax;

	double xcenter = (xmin + xmax) * 0.5;
	double ycenter = (ymin + ymax) * 0.5;

	double xwidth = (xmax - xmin);
	double ywidth = (ymax - ymin);

	xmin -= xwidth * MARGIN_RATIO;
	xmax += xwidth * MARGIN_RATIO;
	ymin -= ywidth * MARGIN_RATIO;
	ymax += ywidth * MARGIN_RATIO;

	RectRegion region2(xmin, xmax, ymin, ymax);

	auto vid = pointLocator->FindClosestPoint(xcenter, ycenter, 0);
	if (vid == -1) {
		*outOfRegion = true;
		return;
	}

	double* cv = data->GetPoint(vid);
	if (*cv < xmin || *cv > xmax || *(cv + 1) < ymin || *(cv + 1) > ymax) {
		*outOfRegion = true;
		return;
	}

	int dimensions[3];
	data->GetDimensions(dimensions);
	int centerI = vid % *(dimensions);
	int centerJ = (vid - centerI) / *(dimensions);

	int iMin1, iMax1, jMin1, jMax1;

	// calc iMin1
	if (pointIsInsideRegion(region2, data, dimensions, 0, centerJ)) {
		iMin1 = 0;
	} else {
		iMin1 = limitIIntersects(region2, data, dimensions, centerI, 0, centerJ);
	}
	// calc iMax1
	if (pointIsInsideRegion(region2, data, dimensions, *(dimensions) - 1, centerJ)) {
		iMax1 = *(dimensions) - 1;
	} else {
		iMax1 = limitIIntersects(region2, data, dimensions, centerI, *(dimensions) - 1, centerJ);
	}
	// calc jMin1
	if (pointIsInsideRegion(region2, data, dimensions, centerI, 0)) {
		jMin1 = 0;
	} else {
		jMin1 = limitJIntersects(region2, data, dimensions, centerJ, 0, centerI);
	}
	// calc jMax1
	if (pointIsInsideRegion(region2, data, dimensions, centerI, *(dimensions + 1) - 1)) {
		jMax1 = *(dimensions + 1) - 1;
	} else {
		jMax1 = limitJIntersects(region2, data, dimensions, centerJ, *(dimensions + 1) - 1, centerI);
	}

	int iMin2, iMax2, jMin2, jMax2;
	// calc iMin2
	if (iMin1 == 0) {
		iMin2 = 0;
	} else {
		if (iLineIntersectsRegion(region2, data, dimensions, 0)) {
			iMin2 = 0;
		} else {
			iMin2 = limitIIntersects(region2, data, dimensions, centerI, 0);
		}
	}
	// calc iMax2
	if (iMax1 == *(dimensions) - 1) {
		iMax2 = iMax1;
	} else {
		if (iLineIntersectsRegion(region2, data, dimensions, *(dimensions) - 1)) {
			iMax2 = *(dimensions) - 1;
		} else {
			iMax2 = limitIIntersects(region2, data, dimensions, centerI, *(dimensions) - 1);
		}
	}
	// calc jMin2
	if (jMin1 == 0) {
		jMin2 = 0;
	} else {
		if (jLineIntersectsRegion(region2, data, dimensions, 0)) {
			jMin2 = 0;
		} else {
			jMin2 = limitJIntersects(region2, data, dimensions, centerJ, 0);
		}
	}
	// calc jMax2
	if (jMax1 == *(dimensions + 1) - 1) {
		jMax2 = jMax1;
	} else {
		jMax2 = limitJIntersects(region2, data, dimensions, centerJ, *(dimensions + 1) - 1);
	}

	*imin = iMin2;
	*imax = iMax2;
	*jmin = jMin2;
	*jmax = jMax2;

	int numCells = (iMax2 - iMin2) * (jMax2 - jMin2);
	*rate = 1;

	while (maxcell > 0 && numCells / (*rate * *rate) > maxcell) {
		*rate += 1;
	}
}

vtkStructuredGrid* vtkPointSetRegionAndCellSizeFilter::extract(vtkStructuredGrid* data, int imin, int imax, int jmin, int jmax, int rate)
{
	auto extract = vtkSmartPointer<vtkExtractGrid>::New();
	extract->SetInputData(data);
	extract->SetVOI(imin, imax, jmin, jmax, 0, 0);
	extract->SetSampleRate(rate, rate, 1);
	extract->Update();

	auto output = extract->GetOutput();
	output->Register(nullptr);

	return output;
}

vtkPointSetRegionAndCellSizeFilter::vtkPointSetRegionAndCellSizeFilter()
{}
