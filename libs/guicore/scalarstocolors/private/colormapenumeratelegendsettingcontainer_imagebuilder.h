#ifndef COLORMAPENUMERATELEGENDSETTINGCONTAINER_IMAGEBUILDER_H
#define COLORMAPENUMERATELEGENDSETTINGCONTAINER_IMAGEBUILDER_H

#include "../colormapenumeratelegendsettingcontainer.h"

#include "../../image/public/imagesettingcontainer_imagebuilder.h"

class ColorMapEnumerateLegendSettingContainer::ImageBuilder : public ImageSettingContainer::ImageBuilder
{
public:
	ImageBuilder(ColorMapEnumerateLegendSettingContainer* setting);

	bool build(QImage* image) override;
	QSize autoSize() const override;

private:
	void buildDiscreteVertical(QPainter* painter, int top, int bottom, QImage* image, const ColorMapEnumerateLegendSettingContainer& s);
	void buildDiscreteHorizontal(QPainter* painter, int top, int bottom, QImage* image, const ColorMapEnumerateLegendSettingContainer& s);

	ColorMapEnumerateLegendSettingContainer* m_setting;
};

#endif // COLORMAPENUMERATELEGENDSETTINGCONTAINER_IMAGEBUILDER_H
