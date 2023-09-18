#ifndef VTKPOINTSETEXTENDEDT_H
#define VTKPOINTSETEXTENDEDT_H

#include "vtkpointsetextended.h"

template <typename V>
class vtkPointSetExtendedT : public vtkPointSetExtended
{
public:
	vtkPointSetExtendedT(V* data, bool twoDimensional, bool geosIndex);

	V* concreteData() const;
};

#include "private/vtkpointsetextendedt_detail.h"

#endif // VTKPOINTSETEXTENDEDT_H
