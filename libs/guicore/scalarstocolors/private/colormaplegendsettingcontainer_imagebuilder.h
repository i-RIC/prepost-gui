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
	QSize autoSize() const override;

private:
	void buildVertical(QPainter* painter, QImage *image, const ColorMapLegendSettingContainer& s);
	void buildContinuousVertical(QPainter* painter, int top, int bottom, QImage* image, const ColorMapLegendSettingContainer& s);
	void buildDiscreteVertical(QPainter* painter, int top, int bottom, QImage* image, const ColorMapLegendSettingContainer& s);

	void buildHorizontal(QPainter* painter, QImage *image, const ColorMapLegendSettingContainer& s);
	void buildContinuousHorizontal(QPainter* painter, int top, int bottom, QImage* image, const ColorMapLegendSettingContainer& s);
	void buildDiscreteHorizontal(QPainter* painter, int top, int bottom, QImage* image, const ColorMapLegendSettingContainer& s);

	ColorMapLegendSettingContainer* m_setting;
};

#endif // COLORMAPLEGENDSETTINGCONTAINER_IMAGEBUILDER_H
