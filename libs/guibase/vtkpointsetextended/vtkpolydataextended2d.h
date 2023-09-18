#ifndef VTKPOLYDATAEXTENDED2D_H
#define VTKPOLYDATAEXTENDED2D_H

#include "../guibase_global.h"
#include "vtkpointsetextendedt.h"

#include <vtkPolyData.h>

class GUIBASEDLL_EXPORT vtkPolyDataExtended2d : public vtkPointSetExtendedT<vtkPolyData>
{
public:
	vtkPolyDataExtended2d();

	vtkPolyData* concreteData() const;
};

#endif // VTKPOLYDATAEXTENDED2D_H
