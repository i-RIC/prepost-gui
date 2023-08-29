#ifndef POST2DWINDOWCELLSCALARGROUPDATAITEM_SETTING_H
#define POST2DWINDOWCELLSCALARGROUPDATAITEM_SETTING_H

#include "../post2dwindowcellscalargroupdataitem.h"

#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/region/region2dsettingcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/opacitycontainer.h>

class Post2dWindowCellScalarGroupDataItem::Setting : public CompositeContainer {
public:
	Setting();
	Setting(const Setting& setting);
	~Setting();

	Setting& operator=(const Setting& setting);
	XmlAttributeContainer& operator=(const XmlAttributeContainer& c) override;

	void load(const QDomNode& node) override;
	void save(QXmlStreamWriter& writer) const override;

	ColorMapSettingContainerI* colorMapSetting;
	Region2dSettingContainer regionSetting;
	OpacityContainer opacity;
};

#endif // POST2DWINDOWCELLSCALARGROUPDATAITEM_SETTING_H
