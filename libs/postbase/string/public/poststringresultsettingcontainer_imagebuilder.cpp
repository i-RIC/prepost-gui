#include "poststringresultsettingcontainer_imagebuilder.h"

#include <QBrush>
#include <QFontMetrics>
#include <QPainter>

namespace {

int OUTER_MARGIN = 10;

} // namespace

PostStringResultSettingContainer::ImageBuilder::ImageBuilder(PostStringResultSettingContainer* setting) :
	ImageSettingContainer::ImageBuilder(),
	m_setting {setting}
{}

bool PostStringResultSettingContainer::ImageBuilder::build(QImage* image)
{
	QPainter painter(image);

	QBrush brush(m_setting->backgroundColor);
	painter.fillRect(QRect(0, 0, image->width() / image->devicePixelRatioF() - 1, image->height() / image->devicePixelRatioF() - 1), brush);

	m_engine.setSetting(*m_setting);
	auto text = m_engine.getValue(m_zoneDataContainer);
	if (text.isEmpty()) {return false;}

	// todo control mergin
	QFontMetrics metrics(m_setting->font);
	auto lines = text.split("\n");
	int lnum = 0;
	for (auto l : lines) {
		painter.drawText(QRect(OUTER_MARGIN, OUTER_MARGIN + metrics.lineSpacing() * lnum, image->width() / image->devicePixelRatioF() - 1, image->height() / image->devicePixelRatioF() - 1), Qt::AlignLeft, l);
		++ lnum;
	}

	return true;
}

QSize PostStringResultSettingContainer::ImageBuilder::autoSize() const
{
	auto text = m_engine.getValue(m_zoneDataContainer);
	if (text.isEmpty()) {
		return QSize();
	}

	QFontMetrics metrics(m_setting->font);
	auto lines = text.split("\n");
	auto content_width = 0;
	for (auto l : lines) {
		content_width = std::max(content_width, metrics.width(l));
	}

	return QSize(OUTER_MARGIN * 2 + content_width, OUTER_MARGIN * 2 + metrics.lineSpacing() * lines.size());
}

void PostStringResultSettingContainer::ImageBuilder::setZoneDataContainer(v4PostZoneDataContainer* c)
{
	m_zoneDataContainer = c;
}
