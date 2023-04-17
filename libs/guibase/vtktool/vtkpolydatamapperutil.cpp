#include "vtkpolydatamapperutil.h"

#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkSmartPointer.h>

vtkPolyDataMapper* vtkPolyDataMapperUtil::createWithScalarVisibilityOff()
{
	auto mapper = vtkPolyDataMapper::New();
	mapper->ScalarVisibilityOff();
	return mapper;
}

vtkPolyDataMapper* vtkPolyDataMapperUtil::createWithScalarVisibilityOffWithEmptyPolyData()
{
	auto polyData = vtkSmartPointer<vtkPolyData>::New();
	auto mapper = createWithScalarVisibilityOff();
	mapper->SetInputData(polyData);
	return mapper;
}

vtkPolyDataMapperUtil::vtkPolyDataMapperUtil()
{}
