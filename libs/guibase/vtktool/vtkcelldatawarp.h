#ifndef VTKCELLDATAWARP_H
#define VTKCELLDATAWARP_H

#include "../guibase_global.h"

class vtkPointSet;
class vtkUnstructuredGrid;

class GUIBASEDLL_EXPORT vtkCellDataWarp
{
public:
	static vtkUnstructuredGrid* warp(vtkPointSet* data);

private:
	vtkCellDataWarp();
};

#endif // VTKCELLDATAWARP_H
