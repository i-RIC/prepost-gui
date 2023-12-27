#include "logosetting_imagebuilder.h"

#include <QPainter>

LogoSetting::ImageBuilder::ImageBuilder(LogoSetting* setting) :
	m_setting {setting},
	m_image {":/libs/guicore/images/logo.png"}
{}

bool LogoSetting::ImageBuilder::build(QImage* image)
{
	QPainter painter(image);

	painter.drawImage(QPoint(0, 0), m_image);
	return true;
}

QSize LogoSetting::ImageBuilder::autoSize() const
{
	return m_image.size();
}
