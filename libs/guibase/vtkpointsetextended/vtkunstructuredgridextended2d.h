#ifndef VTKUNSTRUCTUREDGRIDEXTENDED2D_H
#define VTKUNSTRUCTUREDGRIDEXTENDED2D_H

#include "../guibase_global.h"
#include "vtkpointsetextendedt.h"

#include <vtkUnstructuredGrid.h>

class GUIBASEDLL_EXPORT vtkUnstructuredGridExtended2d : public vtkPointSetExtendedT<vtkUnstructuredGrid>
{
public:
	vtkUnstructuredGridExtended2d();

	vtkUnstructuredGrid* concreteData() const;
};

#endif // VTKUNSTRUCTUREDGRIDEXTENDED2D_H
