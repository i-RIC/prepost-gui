#ifndef V4GRID3DT_DETAIL_H
#define V4GRID3DT_DETAIL_H

#include "../v4grid3dt.h"

template <typename V>
v4Grid3dT<V>::v4Grid3dT() :
	v4Grid3d {new V()}
{}

template <typename V>
V* v4Grid3dT<V>::vtkConcreteData() const
{
	return static_cast<V*> (vtkData());
}

#endif // V4GRID3DT_DETAIL_H
