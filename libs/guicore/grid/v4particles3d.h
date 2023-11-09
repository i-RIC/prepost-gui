#ifndef V4PARTICLES3D_H
#define V4PARTICLES3D_H

#include "../guicore_global.h"

#include "v4grid3dt.h"

#include <guibase/vtkpointsetextended/vtkpolydataextended3d.h>

class GUICOREDLL_EXPORT v4Particles3d : public v4Grid3dT<vtkPolyDataExtended3d>
{
public:
	v4Particles3d();
};

#endif // V4PARTICLES3D_H
