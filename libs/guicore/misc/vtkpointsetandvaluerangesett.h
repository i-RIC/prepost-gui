#ifndef VTKPOINTSETANDVALUERANGESETT_H
#define VTKPOINTSETANDVALUERANGESETT_H

#include "vtkpointsetandvaluerangeset.h"

template <typename V>
class vtkPointSetAndValueRangeSetT : public vtkPointSetAndValueRangeSet
{
public:
	vtkPointSetAndValueRangeSetT(V* data = nullptr);

	V* concreteData() const;
};

#include "private/vtkpointsetandvaluerangesett_detail.h"

#endif // VTKPOINTSETANDVALUERANGESETT_H
