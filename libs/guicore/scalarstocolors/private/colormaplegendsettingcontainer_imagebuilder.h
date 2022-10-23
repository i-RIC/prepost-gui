#ifndef COLORMAPLEGENDSETTINGCONTAINER_IMAGEBUILDER_H
#define COLORMAPLEGENDSETTINGCONTAINER_IMAGEBUILDER_H

#include "../colormaplegendsettingcontainer.h"

#include "../../image/public/imagesettingcontainer_imagebuilder.h"

class QPainter;

class ColorMapLegendSettingContainer::ImageBuilder : public ImageSettingContainer::ImageBuilder
{
public:
	ImageBuilder(ColorMapLegendSettingContainer* setting);

	bool build(QImage* image) override;

private:
	void buildContinuous(QPainter* painter, int top, int bottom, QImage* image);
	void buildDiscrete(QPainter* painter, int top, int bottom, QImage* image);

	ColorMapLegendSettingContainer* m_setting;
};

#endif // COLORMAPLEGENDSETTINGCONTAINER_IMAGEBUILDER_H
