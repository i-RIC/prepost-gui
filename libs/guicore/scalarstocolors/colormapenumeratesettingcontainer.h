#ifndef COLORMAPENUMERATESETTINGCONTAINER_H
#define COLORMAPENUMERATESETTINGCONTAINER_H

#include "../guicore_global.h"
#include "colormapenumeratelegendsettingcontainer.h"
#include "colormapsettingcontaineri.h"
#include "colormapsettingvaluecolorpaircontainer.h"

#include <misc/compositecontainer.h>

#include <map>

class EnumLoader;
class vtkIntArray;

class GUICOREDLL_EXPORT ColorMapEnumerateSettingContainer : public CompositeContainer, public ColorMapSettingContainerI
{
public:
	ColorMapEnumerateSettingContainer();
	ColorMapEnumerateSettingContainer(const ColorMapEnumerateSettingContainer& c);

	void setup(const EnumLoader* enumLoader);

	vtkMapper* buildCellDataMapper(vtkDataSet* data, bool ignoreTransparent) override;
	vtkMapper* buildPointDataMapper(vtkDataSet* data) override;

	ColorMapLegendSettingContainerI* legendSetting() override;
	ColorMapLegendSettingContainerI* copyLegendSetting() override;

	ColorMapEnumerateSettingContainer& operator=(const ColorMapEnumerateSettingContainer& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	void load(const QDomNode& node) override;
	void save(QXmlStreamWriter& writer) const override;

	void copyValue(const XmlAttributeContainer& c) override;

	std::vector<ColorMapSettingValueColorPairContainer> colors;

	ColorMapEnumerateLegendSettingContainer legend;

	std::map<double, QString> valueCaptions;
};

#endif // COLORMAPENUMERATESETTINGCONTAINER_H
