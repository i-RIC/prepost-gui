#ifndef POST3DSAMPLINGRATESETTINGCONTAINER_H
#define POST3DSAMPLINGRATESETTINGCONTAINER_H

#include <misc/compositecontainer.h>
#include <misc/intcontainer.h>

class Post3dSamplingRateSettingContainer : public CompositeContainer
{
public:
	Post3dSamplingRateSettingContainer();
	Post3dSamplingRateSettingContainer(const Post3dSamplingRateSettingContainer& c);
	~Post3dSamplingRateSettingContainer();

	Post3dSamplingRateSettingContainer& operator=(const Post3dSamplingRateSettingContainer& c);
	XmlAttributeContainer& operator=(const  XmlAttributeContainer& c) override;

	IntContainer iSamplingRate;
	IntContainer jSamplingRate;
	IntContainer kSamplingRate;
};

#endif // POST3DSAMPLINGRATESETTINGCONTAINER_H
