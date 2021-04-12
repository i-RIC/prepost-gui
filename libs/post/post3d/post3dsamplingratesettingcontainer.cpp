#include "post3dsamplingratesettingcontainer.h"

Post3dSamplingRateSettingContainer::Post3dSamplingRateSettingContainer() :
	CompositeContainer({&iSamplingRate, &jSamplingRate, &kSamplingRate}),
	iSamplingRate {"iSamplingRate"},
	jSamplingRate {"jSamplingRate"},
	kSamplingRate {"kSamplingRate"}
{}

Post3dSamplingRateSettingContainer::Post3dSamplingRateSettingContainer(const Post3dSamplingRateSettingContainer& c) :
	Post3dSamplingRateSettingContainer()
{
	copyValue(c);
}

Post3dSamplingRateSettingContainer::~Post3dSamplingRateSettingContainer()
{}

Post3dSamplingRateSettingContainer& Post3dSamplingRateSettingContainer::operator=(const Post3dSamplingRateSettingContainer& c)
{
	copyValue(c);
	return *this;
}

XmlAttributeContainer& Post3dSamplingRateSettingContainer::operator=(const  XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const Post3dSamplingRateSettingContainer&>(c));
}
