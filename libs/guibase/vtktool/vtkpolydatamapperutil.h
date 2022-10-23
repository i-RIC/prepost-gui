#ifndef VTKPOLYDATAMAPPERUTIL_H
#define VTKPOLYDATAMAPPERUTIL_H

#include "../guibase_global.h"

class vtkPolyDataMapper;

class GUIBASEDLL_EXPORT vtkPolyDataMapperUtil
{
public:
	static vtkPolyDataMapper* createWithScalarVisibilityOff();

private:
	vtkPolyDataMapperUtil();
};

#endif // VTKPOLYDATAMAPPERUTIL_H
