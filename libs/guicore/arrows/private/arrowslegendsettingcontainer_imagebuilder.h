#ifndef ARROWSLEGENDSETTINGCONTAINER_IMAGEBUILDER_H
#define ARROWSLEGENDSETTINGCONTAINER_IMAGEBUILDER_H

#include "../arrowslegendsettingcontainer.h"
#include "../../image/public/imagesettingcontainer_imagebuilder.h"

class ArrowsLegendSettingContainer::ImageBuilder : public ImageSettingContainer::ImageBuilder
{
public:
	ImageBuilder(ArrowsLegendSettingContainer* setting);

	bool build(QImage* image) override;
	QSize autoSize() const override;

private:
	ArrowsLegendSettingContainer* m_setting;
};

#endif // ARROWSLEGENDSETTINGCONTAINER_IMAGEBUILDER_H
