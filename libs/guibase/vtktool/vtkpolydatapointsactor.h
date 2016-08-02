#ifndef VTKPOLYDATAPOINTSACTOR_H
#define VTKPOLYDATAPOINTSACTOR_H

#include "../guibase_global.h"
#include "vtkactorpolydatamapperpair.h"

class vtkPolyData;

class GUIBASEDLL_EXPORT vtkPolyDataPointsActor : public vtkActorPolyDataMapperPair
{
public:
	vtkPolyDataPointsActor();

	void setPolyData(vtkPolyData* polyData);
};

#endif // VTKPOLYDATAPOINTSACTOR_H
