#ifndef GRIDATTRIBUTECELLCONTAINERT_DETAIL_H
#define GRIDATTRIBUTECELLCONTAINERT_DETAIL_H

#include "../gridattributecellcontainert.h"

#include <vtkCellData.h>
#include <vtkPointSet.h>

template <class V, class DA>
GridAttributeCellContainerT<V, DA>::GridAttributeCellContainerT(Grid* grid, SolverDefinitionGridAttributeT<V>* cond) :
	GridAttributeContainerT<V> (grid, cond)
{
	DA* da = DA::New();
	da->SetName(GridAttributeContainerT<V>::name().c_str());
	grid->vtkGrid()->GetCellData()->AddArray(da);
	da->Delete();
}

template <class V, class DA>
GridAttributeCellContainerT<V, DA>::~GridAttributeCellContainerT()
{}

template <class V, class DA>
unsigned int GridAttributeCellContainerT<V, DA>::dataCount() const
{
	return GridAttributeContainerT<V>::grid()->cellCount();
}

template <class V, class DA>
V GridAttributeCellContainerT<V, DA>::value(unsigned int index) const
{
	return dataArray()->GetValue(static_cast<vtkIdType>(index));
}

template <class V, class DA>
void GridAttributeCellContainerT<V, DA>::setValue(unsigned int index, V val)
{
	dataArray()->SetValue(static_cast<vtkIdType>(index), val);
}

template <class V, class DA>
DA* GridAttributeCellContainerT<V, DA>::dataArray() const
{
	vtkPointSet* ps = const_cast<vtkPointSet*> (GridAttributeContainerT<V>::grid()->vtkGrid());
	vtkDataArray* da = ps->GetCellData()->GetArray(GridAttributeContainerT<V>::name().c_str());
	DA* da2 = DA::SafeDownCast(da);
	return da2;
}

template <class V, class DA>
DA* GridAttributeCellContainerT<V, DA>::dataArrayCopy() const
{
	DA* copy = DA::New();
	DA* orig = dataArray();
	copy->DeepCopy(orig);
	copy->SetName(orig->GetName());
	return copy;
}

template <class V, class DA>
void GridAttributeCellContainerT<V, DA>::allocate()
{
	vtkDataArray* a = GridAttributeContainerT<V>::grid()->vtkGrid()->GetCellData()->GetArray(GridAttributeContainerT<V>::name().c_str());
	DA* da = DA::SafeDownCast(a);
	if (da == nullptr) {
		// not found maybe reset?
		da = DA::New();
		da->SetName(GridAttributeContainerT<V>::name().c_str());
		GridAttributeContainerT<V>::grid()->vtkGrid()->GetCellData()->AddArray(da);
		da->Delete();
	}
	da->Allocate(dataCount());
	int count = dataCount();
	for (int i = 0; i < count; ++i) {
		da->InsertValue(i, 0);
	}
}

#endif // GRIDATTRIBUTECELLCONTAINERT_DETAIL_H
