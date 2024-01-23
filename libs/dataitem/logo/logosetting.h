#ifndef DISTANCEMEASURESETTING_H
#define DISTANCEMEASURESETTING_H

#include <guicore/image/imagesettingcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/enumcontainert.h>

class LogoSetting : public CompositeContainer
{
public:
	enum class ImageSize {
		Small,
		Standard,
		Large
	};

	LogoSetting();
	LogoSetting(const LogoSetting& setting);
	~LogoSetting();

	LogoSetting& operator=(const LogoSetting& setting);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	EnumContainerT<ImageSize> imageSize;
	ImageSettingContainer imageSetting;

private:
	class ImageBuilder;
	ImageBuilder* m_imageBuilder;
};

#endif // DISTANCEMEASURESETTING_H
