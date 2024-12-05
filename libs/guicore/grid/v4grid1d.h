#ifndef V4GRID1D_H
#define V4GRID1D_H

#include "../guicore_global.h"

#include "v4grid.h"

class GUICOREDLL_EXPORT v4Grid1d : public v4Grid
{
public:
	v4Grid1d(vtkPointSetExtended* data);
	~v4Grid1d();

	double point1d(vtkIdType index) const;
	void setPoint1d(vtkIdType index, double v);
};

#endif // V4GRID1D_H
