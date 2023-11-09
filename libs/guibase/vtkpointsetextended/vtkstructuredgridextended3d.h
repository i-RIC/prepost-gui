#ifndef VTKSTRUCTUREDGRIDEXTENDED3D_H
#define VTKSTRUCTUREDGRIDEXTENDED3D_H

#include "../guibase_global.h"
#include "vtkpointsetextendedt.h"

#include <vtkStructuredGrid.h>

class GUIBASEDLL_EXPORT vtkStructuredGridExtended3d : public vtkPointSetExtendedT<vtkStructuredGrid>
{
public:
	vtkStructuredGridExtended3d();

	vtkStructuredGrid* concreteData() const;
};

#endif // VTKSTRUCTUREDGRIDEXTENDED3D_H
