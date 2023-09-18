#ifndef V4GRID2DT_H
#define V4GRID2DT_H

#include "v4grid2d.h"

template <typename V>
class v4Grid2dT : public v4Grid2d
{
public:
	v4Grid2dT();

	V* vtkConcreteData() const;
};

#include "private/v4grid2dt_detail.h"

#endif // V4GRID2DT_H
