#ifndef GRIDATTRIBUTENODECONTAINERT_DETAIL_H
#define GRIDATTRIBUTENODECONTAINERT_DETAIL_H

#include "../gridattributecellcontainert.h"

#include <vtkPointData.h>
#include <vtkPointSet.h>

template <class V, class DA>
GridAttributeNodeContainerT<V, DA>::GridAttributeNodeContainerT(Grid* grid, SolverDefinitionGridAttributeT<V>* cond) :
	GridAttributeContainerT<V> (grid, cond)
{
	DA* da = DA::New();
	da->SetName(GridAttributeContainerT<V>::name().c_str());
	grid->vtkGrid()->GetPointData()->AddArray(da);
	da->Delete();
}

template <class V, class DA>
GridAttributeNodeContainerT<V, DA>::~GridAttributeNodeContainerT()
{}

template <class V, class DA>
unsigned int GridAttributeNodeContainerT<V, DA>::dataCount() const
{
	return GridAttributeContainerT<V>::grid()->nodeCount();
}

template <class V, class DA>
V GridAttributeNodeContainerT<V, DA>::value(unsigned int index) const
{
	return dataArray()->GetValue(static_cast<vtkIdType>(index));
}

template <class V, class DA>
void GridAttributeNodeContainerT<V, DA>::setValue(unsigned int index, V val)
{
	dataArray()->SetValue(static_cast<vtkIdType>(index), val);
}

template <class V, class DA>
DA* GridAttributeNodeContainerT<V, DA>::dataArray() const
{
	vtkPointSet* ps = const_cast<vtkPointSet*> (GridAttributeContainerT<V>::grid()->vtkGrid());
	vtkDataArray* da = ps->GetPointData()->GetArray(GridAttributeContainerT<V>::name().c_str());
	DA* da2 = DA::SafeDownCast(da);
	return da2;
}

template <class V, class DA>
DA* GridAttributeNodeContainerT<V, DA>::dataArrayCopy() const
{
	DA* copy = DA::New();
	copy->DeepCopy(dataArray());
	return copy;
}

template <class V, class DA>
void GridAttributeNodeContainerT<V, DA>::allocate()
{
	vtkDataArray* a = GridAttributeContainerT<V>::grid()->vtkGrid()->GetPointData()->GetArray(GridAttributeContainerT<V>::name().c_str());
	DA* da = DA::SafeDownCast(a);
	if (da == 0) {
		// not found maybe reset?
		da = DA::New();
		da->SetName(GridAttributeContainerT<V>::name().c_str());
		GridAttributeContainerT<V>::grid()->vtkGrid()->GetPointData()->AddArray(da);
		da->Delete();
	}
	da->Allocate(dataCount());
	int count = dataCount();
	for (int i = 0; i < count; ++i) {
		da->InsertValue(i, 0);
	}
}

#endif // GRIDATTRIBUTENODECONTAINERT_DETAIL_H
