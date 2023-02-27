#include "post3dwindowcellrangesettingcontainer.h"

#include <vtkSmartPointer.h>
#include <vtkExtractGrid.h>

Post3dWindowCellRangeSettingContainer::Post3dWindowCellRangeSettingContainer() :
	CompositeContainer({&enabled, &iMin, &iMax, &jMin, &jMax, &kMin, &kMax}),
	enabled {"enabled", true},
	iMin {"iMin"},
	iMax {"iMax"},
	jMin {"jMin"},
	jMax {"jMax"},
	kMin {"kMin"},
	kMax {"kMax"}
{}

Post3dWindowCellRangeSettingContainer::Post3dWindowCellRangeSettingContainer(const Post3dWindowCellRangeSettingContainer& c) :
	Post3dWindowCellRangeSettingContainer()
{
	copyValue(c);
}

Post3dWindowCellRangeSettingContainer::~Post3dWindowCellRangeSettingContainer()
{}

vtkStructuredGrid* Post3dWindowCellRangeSettingContainer::extractRegion(vtkStructuredGrid* grid)
{
	auto eGrid = vtkSmartPointer<vtkExtractGrid>::New();
	eGrid->SetInputData(grid);
	eGrid->SetVOI(iMin, iMax + 1, jMin, jMax + 1, kMin, kMax + 1);
	eGrid->Update();

	auto ret = eGrid->GetOutput();
	ret->Register(nullptr);

	return ret;
}

Post3dWindowCellRangeSettingContainer& Post3dWindowCellRangeSettingContainer::operator=(const Post3dWindowCellRangeSettingContainer& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& Post3dWindowCellRangeSettingContainer::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const Post3dWindowCellRangeSettingContainer&> (c));
}
