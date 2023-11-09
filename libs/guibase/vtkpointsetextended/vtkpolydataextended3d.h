#ifndef VTKPOLYDATAEXTENDED3D_H
#define VTKPOLYDATAEXTENDED3D_H

#include "../guibase_global.h"
#include "vtkpointsetextendedt.h"

#include <vtkPolyData.h>

class GUIBASEDLL_EXPORT vtkPolyDataExtended3d : public vtkPointSetExtendedT<vtkPolyData>
{
public:
	vtkPolyDataExtended3d();

	vtkPolyData* concreteData() const;
};

#endif // VTKPOLYDATAEXTENDED3D_H
