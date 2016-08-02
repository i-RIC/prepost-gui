#ifndef VTKPOLYDATALINESACTOR_H
#define VTKPOLYDATALINESACTOR_H

#include "../guibase_global.h"
#include "vtkactorpolydatamapperpair.h"

class vtkPolyData;

class GUIBASEDLL_EXPORT vtkPolyDataLinesActor : public vtkActorPolyDataMapperPair
{
public:
	vtkPolyDataLinesActor();

	void setPolyData(vtkPolyData* polyData);
};

#endif // VTKPOLYDATALINESACTOR_H
