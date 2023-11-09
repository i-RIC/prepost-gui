#ifndef VTKSTRUCTUREDGRIDEXTENDED2D_H
#define VTKSTRUCTUREDGRIDEXTENDED2D_H

#include "../guibase_global.h"
#include "vtkpointsetextendedt.h"

#include <vtkStructuredGrid.h>

class GUIBASEDLL_EXPORT vtkStructuredGridExtended2d : public vtkPointSetExtendedT<vtkStructuredGrid>
{
public:
	vtkStructuredGridExtended2d();

	vtkStructuredGrid* concreteData() const;
};

#endif // VTKSTRUCTUREDGRIDEXTENDED2D_H
