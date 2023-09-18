#ifndef V4GRID2DT_DETAIL_H
#define V4GRID2DT_DETAIL_H

#include "../v4grid2dt.h"

template <typename V>
v4Grid2dT<V>::v4Grid2dT() :
	v4Grid2d {new V()}
{}

template <typename V>
V* v4Grid2dT<V>::vtkConcreteData() const
{
	return static_cast<V*> (vtkData());
}

#endif // V4GRID2DT_DETAIL_H
