#ifndef V4GRID1DT_DETAIL_H
#define V4GRID1DT_DETAIL_H

#include "../v4grid1dt.h"

template <typename V>
v4Grid1dT<V>::v4Grid1dT() :
	v4Grid1d {new V()}
{}

template <typename V>
V* v4Grid1dT<V>::vtkConcreteData() const
{
	return static_cast<V*> (vtkData());
}

#endif // V4GRID1DT_DETAIL_H
