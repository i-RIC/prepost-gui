#include "delegatedcolormapsettingcontainer.h"

#include <guicore/scalarstocolors/colormaplegendsettingcontaineri.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/image/imagesettingcontainer.h>

DelegatedColorMapSettingContainer::DelegatedColorMapSettingContainer() :
	CompositeContainer ({&usePreSetting}),
	usePreSetting {"usePreSetting", false},
	preSetting {nullptr},
	customSetting {nullptr}
{}

DelegatedColorMapSettingContainer::DelegatedColorMapSettingContainer(const DelegatedColorMapSettingContainer& s) :
	DelegatedColorMapSettingContainer {}
{
	copyValue(s);
	customSetting->copy(*s.customSetting);
}

DelegatedColorMapSettingContainer::~DelegatedColorMapSettingContainer()
{
	delete preSetting;
	delete customSetting;
}

DelegatedColorMapSettingContainer& DelegatedColorMapSettingContainer::operator=(const DelegatedColorMapSettingContainer& c)
{
	copyValue(c);
	customSetting->copy(*c.customSetting);
	return *this;
}

XmlAttributeContainer& DelegatedColorMapSettingContainer::operator=(const XmlAttributeContainer& c)
{
	return operator=(dynamic_cast<const DelegatedColorMapSettingContainer&> (c));
}

void DelegatedColorMapSettingContainer::load(const QDomNode& node)
{
	CompositeContainer::load(node);
	customSetting->load(node);
}

void DelegatedColorMapSettingContainer::save(QXmlStreamWriter& writer) const
{
	CompositeContainer::save(writer);
	customSetting->save(writer);
}

ImageSettingContainer* DelegatedColorMapSettingContainer::activeImageSetting() const
{
	if (usePreSetting) {
		return preSetting->imgSetting();
	} else {
		return customSetting->legendSetting()->imgSetting();
	}
}
