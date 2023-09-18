#ifndef VTKPOINTSETEXTENDEDT_DETAIL_H
#define VTKPOINTSETEXTENDEDT_DETAIL_H

#include "../vtkpointsetextendedt.h"
#include "vtkpointsetextended_impl.h"

template <typename V>
vtkPointSetExtendedT<V>::vtkPointSetExtendedT(V* data, bool twoDimensional, bool geosIndex) :
	vtkPointSetExtended {data, twoDimensional, geosIndex}
{}

template <typename V>
V* vtkPointSetExtendedT<V>::concreteData() const
{
	return V::SafeDownCast(impl->m_data);
}

#endif // VTKPOINTSETEXTENDEDT_DETAIL_H
