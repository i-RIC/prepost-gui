#include "vtkpolydatamapperutil.h"

#include <vtkPolyDataMapper.h>

vtkPolyDataMapper* vtkPolyDataMapperUtil::createWithScalarVisibilityOff()
{
	auto mapper = vtkPolyDataMapper::New();
	mapper->ScalarVisibilityOff();
	return mapper;
}

vtkPolyDataMapperUtil::vtkPolyDataMapperUtil()
{}
