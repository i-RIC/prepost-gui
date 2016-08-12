#ifndef VTKPOLYDATAPAINTACTOR_H
#define VTKPOLYDATAPAINTACTOR_H

#include "../guibase_global.h"
#include "vtkactorpolydatamapperpair.h"

class vtkPolyData;

class GUIBASEDLL_EXPORT vtkPolyDataPaintActor : public vtkActorPolyDataMapperPair
{
public:
	vtkPolyDataPaintActor();

	void setPolyData(vtkPolyData* polyData);
};

#endif // VTKPOLYDATAPAINTACTOR_H
