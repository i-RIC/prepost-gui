#ifndef VTKPOINTSETGEOS2DINDEX_H
#define VTKPOINTSETGEOS2DINDEX_H

#include "../guibase_global.h"

class vtkCell;
class vtkPointSet;

class GUIBASEDLL_EXPORT vtkPointSetGeos2dIndex
{
public:
	vtkPointSetGeos2dIndex(vtkPointSet* ps);
	~vtkPointSetGeos2dIndex();

	vtkPointSet* data() const;

	vtkCell* findCell(double x, double y, double* weights);

private:
	class Impl;
	Impl* impl;
};

#endif // VTKPOINTSETGEOS2DINDEX_H
