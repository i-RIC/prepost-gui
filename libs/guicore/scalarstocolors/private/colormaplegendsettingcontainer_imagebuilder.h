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
	void buildVertical(QPainter* painter, QImage *image);
	void buildContinuousVertical(QPainter* painter, int top, int bottom, QImage* image);
	void buildDiscreteVertical(QPainter* painter, int top, int bottom, QImage* image);

	void buildHorizontal(QPainter* painter, QImage *image);
	void buildContinuousHorizontal(QPainter* painter, int top, int bottom, QImage* image);
	void buildDiscreteHorizontal(QPainter* painter, int top, int bottom, QImage* image);

	ColorMapLegendSettingContainer* m_setting;
};

#endif // COLORMAPLEGENDSETTINGCONTAINER_IMAGEBUILDER_H
