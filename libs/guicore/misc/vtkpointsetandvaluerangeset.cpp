#include "vtkpointsetandvaluerangeset.h"

vtkPointSetAndValueRangeSet::vtkPointSetAndValueRangeSet(vtkPointSet* data) :
	m_data {data},
	m_valueRangeSet{}
{}

vtkPointSetAndValueRangeSet::~vtkPointSetAndValueRangeSet()
{
	m_data->UnRegister(nullptr);
}

vtkPointSet* vtkPointSetAndValueRangeSet::data() const
{
	return m_data;
}

const vtkPointSetValueRangeSet& vtkPointSetAndValueRangeSet::valueRangeSet() const
{
	return m_valueRangeSet;
}

void vtkPointSetAndValueRangeSet::updateValueRangeSet()
{
	m_valueRangeSet.update(m_data);
}
