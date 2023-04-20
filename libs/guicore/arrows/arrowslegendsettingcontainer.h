#ifndef ARROWSLEGENDSETTINGCONTAINER_H
#define ARROWSLEGENDSETTINGCONTAINER_H

#include "../guicore_global.h"
#include "../image/imagesettingcontainer.h"

#include <misc/boolcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/colorcontainer.h>
#include <misc/doublecontainer.h>
#include <misc/enumcontainert.h>
#include <misc/qfontcontainer.h>
#include <misc/opacitycontainer.h>
#include <misc/stringcontainer.h>

class ArrowsSettingContainer;

class GUICOREDLL_EXPORT ArrowsLegendSettingContainer : public CompositeContainer
{
public:
	ArrowsLegendSettingContainer();
	ArrowsLegendSettingContainer(const ArrowsLegendSettingContainer& c);
	~ArrowsLegendSettingContainer();

	void setArrowsSetting(ArrowsSettingContainer* c);

	ArrowsLegendSettingContainer& operator=(const ArrowsLegendSettingContainer& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	BoolContainer visible;

	StringContainer title;
	QFontContainer titleFont;

	StringContainer lengthFormat;
	QFontContainer lengthFont;

	ColorContainer titleColor;
	ColorContainer lengthColor;
	ColorContainer backgroundColor;
	OpacityContainer backgroundOpacity;

	ImageSettingContainer imageSetting;

private:
	ArrowsSettingContainer* m_arrowsSetting;

	class ImageBuilder;
	ImageBuilder* m_imageBuilder;
};

#endif // ARROWSLEGENDSETTINGCONTAINER_H
