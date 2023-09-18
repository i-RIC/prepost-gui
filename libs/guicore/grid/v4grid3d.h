#ifndef V4GRID3D_H
#define V4GRID3D_H

#include "../guicore_global.h"

#include "v4grid.h"

class GUICOREDLL_EXPORT v4Grid3d : public v4Grid
{
public:
	v4Grid3d(vtkPointSetExtended* data);

	virtual vtkPointSet* vtkIndexData() const;
};

#endif // V4GRID3D_H
