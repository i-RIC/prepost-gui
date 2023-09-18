#ifndef V4PARTICLES2D_H
#define V4PARTICLES2D_H

#include "../guicore_global.h"

#include "v4grid2dt.h"

#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>

class GUICOREDLL_EXPORT v4Particles2d : public v4Grid2dT<vtkPolyDataExtended2d>
{
public:
	v4Particles2d();

private:
	void updateFilteredData(double xMin, double xMax, double yMin, double yMax) override;
};

#endif // V4PARTICLES2D_H
