#ifndef DELEGATEDCOLORMAPSETTINGCONTAINER_H
#define DELEGATEDCOLORMAPSETTINGCONTAINER_H

#include "../guicore_global.h"

#include <misc/boolcontainer.h>
#include <misc/compositecontainer.h>

class ColorMapLegendSettingContainerI;
class ColorMapSettingContainerI;
class VTKGraphicsView;

class GUICOREDLL_EXPORT DelegatedColorMapSettingContainer : public CompositeContainer
{
public:
	DelegatedColorMapSettingContainer();
	DelegatedColorMapSettingContainer(const DelegatedColorMapSettingContainer& s);
	~DelegatedColorMapSettingContainer();

	DelegatedColorMapSettingContainer& operator=(const DelegatedColorMapSettingContainer& c);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	void load(const QDomNode& node) override;
	void save(QXmlStreamWriter& writer) const override;

	void applyLegendImageSetting(VTKGraphicsView* view);

	BoolContainer usePreSetting;
	ColorMapLegendSettingContainerI* preSetting;
	ColorMapSettingContainerI* customSetting;
};

#endif // DELEGATEDCOLORMAPSETTINGCONTAINER_H
