#ifndef VTKPOINTSETANDVALUERANGESETT_DETAIL_H
#define VTKPOINTSETANDVALUERANGESETT_DETAIL_H

#include "../vtkpointsetandvaluerangesett.h"

template <typename V>
vtkPointSetAndValueRangeSetT<V>::vtkPointSetAndValueRangeSetT(V* data) :
	vtkPointSetAndValueRangeSet {data}
{
	if (m_data == nullptr) {
		m_data = V::New();
	} else {
		m_data->Register(nullptr);
		updateValueRangeSet();
	}
}

template <typename V>
V* vtkPointSetAndValueRangeSetT<V>::concreteData() const
{
	return V::SafeDownCast(m_data);
}

#endif // VTKPOINTSETANDVALUERANGESETT_DETAIL_H
