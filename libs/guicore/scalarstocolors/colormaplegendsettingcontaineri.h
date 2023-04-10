#ifndef COLORMAPLEGENDSETTINGCONTAINERI_H
#define COLORMAPLEGENDSETTINGCONTAINERI_H

class ColorMapSettingContainerI;
class ImageSettingContainer;

class ColorMapLegendSettingContainerI
{
public:
	virtual ~ColorMapLegendSettingContainerI() {};

	virtual void copy(const ColorMapLegendSettingContainerI& setting) = 0;
	virtual void setTitle(const QString& title) = 0;
	virtual ColorMapSettingContainerI* setting() const = 0;
	virtual void setSetting(ColorMapSettingContainerI* setting) = 0;
	virtual ImageSettingContainer* imgSetting() = 0;
};

#endif // COLORMAPLEGENDSETTINGCONTAINERI_H
