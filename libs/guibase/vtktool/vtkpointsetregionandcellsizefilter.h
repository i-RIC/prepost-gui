#ifndef VTKPOINTSETREGIONANDCELLSIZEFILTER_H
#define VTKPOINTSETREGIONANDCELLSIZEFILTER_H

#include "../guibase_global.h"

class RectRegion;

class vtkAbstractPointLocator;
class vtkPointSet;
class vtkPolyData;
class vtkStructuredGrid;

class GUIBASEDLL_EXPORT vtkPointSetRegionAndCellSizeFilter
{
public:
	static vtkPolyData* filterGeneral(vtkPointSet* data, const RectRegion& region, int maxcell, bool* masked);
	static void calcExtractParameters(vtkStructuredGrid* data, const RectRegion& region, vtkAbstractPointLocator* pointLocator, int maxcell, bool* outOfRegion, int* imin, int* imax, int* jmin, int* jmax, int* rate);
	static vtkStructuredGrid* extract(vtkStructuredGrid* data, int imin, int imax, int jmin, int jmax, int rate);

private:
	vtkPointSetRegionAndCellSizeFilter();
};

#endif // VTKPOINTSETREGIONANDCELLSIZEFILTER_H
