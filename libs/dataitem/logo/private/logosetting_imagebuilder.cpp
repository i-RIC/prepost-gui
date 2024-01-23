#include "logosetting_imagebuilder.h"

#include <QPainter>

LogoSetting::ImageBuilder::ImageBuilder(LogoSetting* setting) :
	m_setting {setting}
{}

bool LogoSetting::ImageBuilder::build(QImage* img)
{
	QPainter painter(img);

	auto i = image(m_setting->imageSize);
	painter.drawImage(QPoint(0, 0), i);
	return true;
}

QSize LogoSetting::ImageBuilder::autoSize() const
{
	auto i = image(m_setting->imageSize);
	return i.size();
}

QImage LogoSetting::ImageBuilder::image(ImageSize size) const
{
	return QImage(imageFileName(size));
}

QString LogoSetting::ImageBuilder::imageFileName(ImageSize size) const
{
	QString sizeStr;
	if (size == ImageSize::Small) {
		sizeStr = "40";
	} else if (size == ImageSize::Standard) {
		sizeStr = "80";
	} else if (size == ImageSize::Large) {
		sizeStr = "160";
	}
	return QString(":/libs/dataitem/logo/images/logo_%0.png").arg(sizeStr);
}
