#ifndef V4GRID3DT_H
#define V4GRID3DT_H

#include "v4grid3d.h"

template <typename V>
class v4Grid3dT : public v4Grid3d
{
public:
	v4Grid3dT();

	V* vtkConcreteData() const;
};

#include "private/v4grid3dt_detail.h"

#endif // V4GRID3DT_H
