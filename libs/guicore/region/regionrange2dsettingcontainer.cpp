#include "regionrange2dsettingcontainer.h"

#include <vtkExtractGrid.h>
#include <vtkSmartPointer.h>

RegionRange2dSettingContainer::RegionRange2dSettingContainer() :
	CompositeContainer {&iMin, &iMax, &jMin, &jMax},
	iMin {"iMin"},
	iMax {"iMax"},
	jMin {"jMin"},
	jMax {"jMax"}
{}

RegionRange2dSettingContainer::RegionRange2dSettingContainer(const RegionRange2dSettingContainer& s) :
	RegionRange2dSettingContainer {}
{
	copyValue(s);
}

RegionRange2dSettingContainer& RegionRange2dSettingContainer::operator=(const RegionRange2dSettingContainer& s)
{
	copyValue(s);
	return *this;
}

XmlAttributeContainer& RegionRange2dSettingContainer::operator=(const XmlAttributeContainer& s)
{
	return operator=(dynamic_cast<const RegionRange2dSettingContainer&> (s));
}

vtkStructuredGrid* RegionRange2dSettingContainer::buildNodeFilteredData(vtkStructuredGrid* data) const
{
	auto extract = vtkSmartPointer<vtkExtractGrid>::New();
	extract->SetInputData(data);
	extract->SetVOI(iMin, iMax, jMin, jMax, 0, 0);
	extract->Update();
	auto ret = extract->GetOutput();

	ret->Register(nullptr);
	return ret;
}

vtkStructuredGrid* RegionRange2dSettingContainer::buildCellFilteredData(vtkStructuredGrid* data) const
{
	auto extract = vtkSmartPointer<vtkExtractGrid>::New();
	extract->SetInputData(data);
	extract->SetVOI(iMin, iMax + 1, jMin, jMax + 1, 0, 0);
	extract->Update();
	auto ret = extract->GetOutput();

	ret->Register(nullptr);
	return ret;
}
