#include "post3dcellrangesettingcontainer.h"

#include <vtkSmartPointer.h>
#include <vtkExtractGrid.h>

Post3dCellRangeSettingContainer::Post3dCellRangeSettingContainer() :
	CompositeContainer({&enabled, &iMin, &iMax, &jMin, &jMax, &kMin, &kMax}),
	enabled {"enabled", true},
	iMin {"iMin"},
	iMax {"iMax"},
	jMin {"jMin"},
	jMax {"jMax"},
	kMin {"kMin"},
	kMax {"kMax"}
{}

Post3dCellRangeSettingContainer::Post3dCellRangeSettingContainer(const Post3dCellRangeSettingContainer& c) :
	Post3dCellRangeSettingContainer()
{
	copyValue(c);
}

Post3dCellRangeSettingContainer::~Post3dCellRangeSettingContainer()
{}

vtkStructuredGrid* Post3dCellRangeSettingContainer::extractRegion(vtkStructuredGrid* grid)
{
	auto eGrid = vtkSmartPointer<vtkExtractGrid>::New();
	eGrid->SetInputData(grid);
	eGrid->SetVOI(iMin, iMax + 1, jMin, jMax + 1, kMin, kMax + 1);
	eGrid->Update();

	auto ret = eGrid->GetOutput();
	ret->Register(nullptr);

	return ret;
}

Post3dCellRangeSettingContainer& Post3dCellRangeSettingContainer::operator=(const Post3dCellRangeSettingContainer& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& Post3dCellRangeSettingContainer::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const Post3dCellRangeSettingContainer&> (c));
}
