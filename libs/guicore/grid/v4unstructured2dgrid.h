#ifndef V4UNSTRUCTURED2DGRID_H
#define V4UNSTRUCTURED2DGRID_H

#include "../guicore_global.h"

#include "v4grid2dt.h"

#include <guibase/vtkpointsetextended/vtkunstructuredgridextended2d.h>

class GUICOREDLL_EXPORT v4Unstructured2dGrid : public v4Grid2dT<vtkUnstructuredGridExtended2d>
{
public:
	v4Unstructured2dGrid();

	void updateFilteredData(double xMin, double xMax, double yMin, double yMax) override;
};

#endif // V4UNSTRUCTURED2DGRID_H
