#ifndef VTKPOINTSETEXTENDED_H
#define VTKPOINTSETEXTENDED_H

#include "../guibase_global.h"

#include <vtkIdList.h>

class vtkAbstractPointLocator;
class vtkCell;
class vtkPointSet;
class vtkPointSetValueRangeSet;

class GUIBASEDLL_EXPORT vtkPointSetExtended
{
public:
	vtkPointSetExtended(vtkPointSet* data, bool twoDimensional, bool geosIndex);
	~vtkPointSetExtended();

	vtkPointSet* data() const;
	const vtkPointSetValueRangeSet& valueRangeSet() const;

	bool pointScalarValueExists() const;
	bool cellScalarValueExists() const;

	vtkIdType findClosestPoint(double x, double y, double z) const;
	vtkIdType findClosestPointWithinRadius(double radius, double x, double y, double z) const;
	vtkIdType findCell(double x, double y, double z, double radius, double* weights);

	vtkAbstractPointLocator* pointLocator() const;

	void updateCellIndex();
	void updateValueRangeSet();

protected:
	class Impl;
	Impl* impl;
};

#endif // VTKPOINTSETEXTENDED_H
