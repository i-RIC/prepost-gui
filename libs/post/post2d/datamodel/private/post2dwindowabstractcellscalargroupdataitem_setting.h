#ifndef POST2DWINDOWABSTRACTCELLSCALARGROUPDATAITEM_SETTING_H
#define POST2DWINDOWABSTRACTCELLSCALARGROUPDATAITEM_SETTING_H

#include "../post2dwindowabstractcellscalargroupdataitem.h"

#include <guicore/scalarstocolors/colormapsettingcontainer.h>
#include <guicore/region/region2dsettingcontainer.h>
#include <misc/compositecontainer.h>
#include <misc/intcontainer.h>
#include <misc/opacitycontainer.h>

class Post2dWindowAbstractCellScalarGroupDataItem::Setting : public CompositeContainer {
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
	IntContainer lineWidth;
	OpacityContainer opacity;
};

#endif // POST2DWINDOWABSTRACTCELLSCALARGROUPDATAITEM_SETTING_H
