#include "post3dwindowfacesettingcontainer.h"

#include <vtkExtractGrid.h>
#include <vtkSmartPointer.h>

Post3dWindowFaceSettingContainer::Post3dWindowFaceSettingContainer() :
	CompositeContainer({&enabled, &direction, &iMin, &iMax, &jMin, &jMax, &kMin, &kMax}),
	enabled {"enabled", true},
	direction {"direction", Post3dWindowFaceSettingContainer::Direction::dirI},
	iMin {"iMin"},
	iMax {"iMax"},
	jMin {"jMin"},
	jMax {"jMax"},
	kMin {"kMin"},
	kMax {"kMax"}
{}

Post3dWindowFaceSettingContainer::Post3dWindowFaceSettingContainer(const Post3dWindowFaceSettingContainer& c) :
	Post3dWindowFaceSettingContainer()
{
	copyValue(c);
}

Post3dWindowFaceSettingContainer::~Post3dWindowFaceSettingContainer()
{}

vtkStructuredGrid* Post3dWindowFaceSettingContainer::extractFace(vtkStructuredGrid* grid) const
{
	auto filter = vtkSmartPointer<vtkExtractGrid>::New();
	filter->SetInputData(grid);
	filter->SetVOI(iMin, iMax, jMin, jMax, kMin, kMax);
	filter->Update();
	auto output = filter->GetOutput();
	output->Register(nullptr);

	return output;
}

Post3dWindowFaceSettingContainer& Post3dWindowFaceSettingContainer::operator=(const Post3dWindowFaceSettingContainer& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& Post3dWindowFaceSettingContainer::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const Post3dWindowFaceSettingContainer&> (c));
}
