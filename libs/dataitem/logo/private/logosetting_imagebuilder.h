#ifndef LOGOSETTING_IMAGEBUILDER_H
#define LOGOSETTING_IMAGEBUILDER_H

#include "../logosetting.h"
#include <guicore/image/public/imagesettingcontainer_imagebuilder.h>

#include <QImage>

class LogoSetting::ImageBuilder : public ImageSettingContainer::ImageBuilder
{
public:
	ImageBuilder(LogoSetting* setting);

	bool build(QImage* image) override;
	QSize autoSize() const override;

private:
	QImage image(ImageSize size) const;
	QString imageFileName(ImageSize size) const;

	LogoSetting* m_setting;
};

#endif // LOGOSETTING_IMAGEBUILDER_H
