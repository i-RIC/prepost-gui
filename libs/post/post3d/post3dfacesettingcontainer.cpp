#include "post3dfacesettingcontainer.h"

Post3dFaceSettingContainer::Post3dFaceSettingContainer() :
	CompositeContainer({&enabled, &direction, &iMin, &iMax, &jMin, &jMax, &kMin, &kMax}),
	enabled {"enabled", true},
	direction {"direction"},
	iMin {"iMin"},
	iMax {"iMax"},
	jMin {"jMin"},
	jMax {"jMax"},
	kMin {"kMin"},
	kMax {"kMax"}
{
}

Post3dFaceSettingContainer::Post3dFaceSettingContainer(const Post3dFaceSettingContainer& c) :
	Post3dFaceSettingContainer()
{
	copyValue(c);
}

Post3dFaceSettingContainer::~Post3dFaceSettingContainer()
{}

Post3dFaceSettingContainer& Post3dFaceSettingContainer::operator=(const Post3dFaceSettingContainer& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& Post3dFaceSettingContainer::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const Post3dFaceSettingContainer&> (c));
}
