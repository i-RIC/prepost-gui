#ifndef DISTANCEMEASURESETTING_H
#define DISTANCEMEASURESETTING_H

#include <guicore/image/imagesettingcontainer.h>
#include <misc/compositecontainer.h>

class LogoSetting : public CompositeContainer
{
public:

	LogoSetting();
	LogoSetting(const LogoSetting& setting);
	~LogoSetting();

	LogoSetting& operator=(const LogoSetting& setting);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	ImageSettingContainer imageSetting;

private:
	class ImageBuilder;
	ImageBuilder* m_imageBuilder;
};

#endif // DISTANCEMEASURESETTING_H
