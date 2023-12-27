#include "logosetting.h"
#include "private/logosetting_imagebuilder.h"

#include <QColor>

LogoSetting::LogoSetting() :
	CompositeContainer({&imageSetting}),
	m_imageBuilder {new ImageBuilder {this}}
{
	imageSetting.position = ImageSettingContainer::Position::BottomRight;
	imageSetting.horizontalMargin = 0.01;
	imageSetting.verticalMargin = 0.01;
	imageSetting.autoSize = true;

	imageSetting.setAutoSizeForced(true);
	imageSetting.setNegativePositionForbidden(true);
	imageSetting.setImageBuilder(m_imageBuilder);
}

LogoSetting::LogoSetting(const LogoSetting& setting) :
	LogoSetting {}
{
	copyValue(setting);
}

LogoSetting::~LogoSetting()
{
	delete m_imageBuilder;
}

LogoSetting& LogoSetting::operator=(const LogoSetting& setting)
{
	copyValue(setting);
	return *this;
}

XmlAttributeContainer& LogoSetting::operator=(const XmlAttributeContainer& c)
{
	return operator =(dynamic_cast<const LogoSetting&> (c));
}
