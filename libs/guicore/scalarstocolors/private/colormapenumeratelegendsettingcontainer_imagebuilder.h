#ifndef COLORMAPENUMERATELEGENDSETTINGCONTAINER_IMAGEBUILDER_H
#define COLORMAPENUMERATELEGENDSETTINGCONTAINER_IMAGEBUILDER_H

#include "../colormapenumeratelegendsettingcontainer.h"

#include "../../image/public/imagesettingcontainer_imagebuilder.h"

class ColorMapEnumerateLegendSettingContainer::ImageBuilder : public ImageSettingContainer::ImageBuilder
{
public:
	ImageBuilder(ColorMapEnumerateLegendSettingContainer* setting);

	bool build(QImage* image) override;

private:
	void buildDiscrete(QPainter* painter, int top, int bottom, QImage* image);

	ColorMapEnumerateLegendSettingContainer* m_setting;
};

#endif // COLORMAPENUMERATELEGENDSETTINGCONTAINER_IMAGEBUILDER_H
