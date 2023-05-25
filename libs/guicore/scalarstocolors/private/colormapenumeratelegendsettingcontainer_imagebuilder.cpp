#include "../colormapenumeratesettingcontainer.h"
#include "colormapenumeratelegendsettingcontainer_imagebuilder.h"

namespace {

int OUTER_MARGIN = 12;
int INNER_MARGIN = 10;
int COLOR_VALUE_MARGIN = 10;
int LABEL_MARGIN = 5;

} // namespace

ColorMapEnumerateLegendSettingContainer::ImageBuilder::ImageBuilder(ColorMapEnumerateLegendSettingContainer* setting) :
	m_setting {setting}
{}

bool ColorMapEnumerateLegendSettingContainer::ImageBuilder::build(QImage* image)
{
	ColorMapEnumerateLegendSettingContainer s;
	if (m_setting->delegateMode()) {
		s = m_setting->colorMapSetting()->legend;
		s.visible = m_setting->visible;
	} else {
		s = *m_setting;
	}
	s.setColorMapSetting(m_setting->colorMapSetting());

	if (! s.visible) {return false;}

	QPainter painter(image);

	auto bgCol = s.backgroundColor.value();
	QPen pen;
	if (s.showBorder) {
		pen = QPen(bgCol);
	} else {
		pen = QPen(Qt::transparent);
	}
	double opacity = s.backgroundOpacity.value() / 100.0;
	QBrush brush(QColor(bgCol.red(), bgCol.green(), bgCol.blue(), static_cast<int>(255 * opacity)));
	painter.setPen(pen);
	painter.setBrush(brush);
	painter.drawRect(0, 0, image->width() / image->devicePixelRatioF() - 1, image->height() / image->devicePixelRatioF() - 1);

	painter.setPen(s.titleColor);
	painter.setFont(s.titleFont);
	QFontMetrics metrics(s.titleFont);

	QRect rect(OUTER_MARGIN, OUTER_MARGIN, image->width() / image->devicePixelRatioF() - OUTER_MARGIN * 2, metrics.height());

	Qt::AlignmentFlag align = Qt::AlignHCenter;
	if (s.barAlign == BarAlign::Left) {
		align = Qt::AlignLeft;
	} else if (s.barAlign == BarAlign::Center) {
		align = Qt::AlignHCenter;
	} else if (s.barAlign == BarAlign::Right) {
		align = Qt::AlignRight;
	}

	painter.drawText(rect, align, s.title);

	int bar_top = OUTER_MARGIN + painter.fontMetrics().height() + INNER_MARGIN;
	int bar_bottom = image->height() / image->devicePixelRatioF() - OUTER_MARGIN;

	if (s.direction == Direction::Vertical) {
		buildDiscreteVertical(&painter, bar_top, bar_bottom, image, s);
	} else {
		buildDiscreteHorizontal(&painter, bar_top, bar_bottom, image, s);
	}

	return true;
}

void ColorMapEnumerateLegendSettingContainer::ImageBuilder::buildDiscreteVertical(QPainter* painter, int top, int bottom, QImage* image, const ColorMapEnumerateLegendSettingContainer& s)
{
	int maxLabelWidth = 0;

	auto cs = s.m_colorMapSetting;
	if (cs->colors.size() == 0) {return;}

	QFontMetrics metrics(s.labelFont);

	const auto& cols = cs->colors;

	for (const auto& c : cols) {
		QString caption;
		auto it = cs->valueCaptions.find(c.value.value());
		if (it != cs->valueCaptions.end()) {
			caption = it->second;
		}
		auto rect = metrics.boundingRect(caption);
		if (rect.width() > maxLabelWidth) {
			maxLabelWidth = rect.width();
		}
	}
	maxLabelWidth += LABEL_MARGIN;

	int colorWidth = 0;
	if (s.barAutoWidth) {
		colorWidth = image->width() / image->devicePixelRatioF() - OUTER_MARGIN * 2 - COLOR_VALUE_MARGIN - maxLabelWidth;
	} else {
		colorWidth = s.barWidth;
	}

	double barHeight = (bottom - top) / (cols.size());

	// colors
	double barLeft = 0;
	if (s.barAlign == BarAlign::Left) {
		barLeft = OUTER_MARGIN;
	} else if (s.barAlign == BarAlign::Center) {
		barLeft = image->width() / image->devicePixelRatioF() / 2 - (maxLabelWidth + COLOR_VALUE_MARGIN + colorWidth) / 2.0;
	} else if (s.barAlign == BarAlign::Right) {
		barLeft = image->width() / image->devicePixelRatioF() - OUTER_MARGIN - maxLabelWidth - COLOR_VALUE_MARGIN - colorWidth;
	}
	for (int i = 0; i < cols.size(); ++i) {
		auto c = cols.at(i);
		auto col = c.color;

		int barBottom = image->height() / image->devicePixelRatioF() - OUTER_MARGIN - barHeight * i;
		QBrush brush(col);
		if (c.transparent) {
			QPen pen(brush, 2);
			QBrush tbrush(Qt::transparent);
			painter->setPen(pen);
			painter->setBrush(tbrush);

			QRectF rect(barLeft + 1, barBottom - barHeight + 1, colorWidth - 2, barHeight - 2);
			painter->drawRect(rect);
		} else {
			QRectF rect(barLeft, barBottom - barHeight, colorWidth, barHeight);
			painter->fillRect(rect, brush);
		}
	}

	// labels
	double labelLeft = barLeft + colorWidth + COLOR_VALUE_MARGIN;
	double labelWidth = image->width() / image->devicePixelRatioF() - labelLeft - OUTER_MARGIN;
	if (labelWidth < 0) {labelWidth = 0;}

	QPen labelPen(s.labelColor);
	QFont labelFont(s.labelFont);
	painter->setPen(labelPen);
	painter->setFont(labelFont);

	for (int i = 0; i < cols.size(); ++i) {
		const auto& c = cols.at(i);
		QString caption;
		auto it = cs->valueCaptions.find(c.value.value());
		if (it != cs->valueCaptions.end()) {
			caption = it->second;
		}
		auto rect = metrics.boundingRect(caption);

		int bottom = image->height() / image->devicePixelRatioF() - OUTER_MARGIN - barHeight * (i + 0.5);

		QRectF textRect(labelLeft, bottom - rect.height() * 0.5, labelWidth, rect.height() + 2);
		painter->drawText(textRect, caption);
	}
}

void ColorMapEnumerateLegendSettingContainer::ImageBuilder::buildDiscreteHorizontal(QPainter* painter, int top, int bottom, QImage* image, const ColorMapEnumerateLegendSettingContainer& s)
{
	int maxLabelWidth = 0;
	int labelHeight = 0;

	auto cs = s.m_colorMapSetting;
	if (cs->colors.size() == 0) {return;}

	QFontMetrics metrics(s.labelFont);

	const auto& cols = cs->colors;

	for (const auto& c : cols) {
		QString caption;
		auto it = cs->valueCaptions.find(c.value.value());
		if (it != cs->valueCaptions.end()) {
			caption = it->second;
		}
		auto rect = metrics.boundingRect(caption);
		if (rect.width() > maxLabelWidth) {
			maxLabelWidth = rect.width();
		}

		labelHeight = rect.height();
	}
	maxLabelWidth += LABEL_MARGIN;

	int colorHeight = (bottom - top) - labelHeight - LABEL_MARGIN;
	double barWidth = (image->width() / image->devicePixelRatioF() - OUTER_MARGIN * 2) / static_cast<double>(cols.size());

	// colors
	double barLeftStart = 0;
	if (s.barAlign == BarAlign::Left) {
		barLeftStart = OUTER_MARGIN;
	} else if (s.barAlign == BarAlign::Center) {
		barLeftStart = image->width() / image->devicePixelRatioF() / 2 - (barWidth * cols.size()) / 2;
	} else if (s.barAlign == BarAlign::Right) {
		barLeftStart = image->width() / image->devicePixelRatioF() - OUTER_MARGIN - (barWidth * cols.size());
	}

	for (int i = 0; i < cols.size(); ++i) {
		auto c = cols.at(i);
		auto col = c.color;

		double barLeft = barLeftStart + barWidth * i;
		QBrush brush(col);
		if (c.transparent) {
			QPen pen(brush, 2);
			QBrush tbrush(Qt::transparent);
			painter->setPen(pen);
			painter->setBrush(tbrush);

			QRectF rect(barLeft + 1, top + labelHeight + LABEL_MARGIN + 1, barWidth - 2, colorHeight - 2);
			painter->drawRect(rect);
		} else {
			QRectF rect(barLeft, top + labelHeight + LABEL_MARGIN, barWidth, colorHeight);
			painter->fillRect(rect, brush);
		}
	}

	// labels
	QPen labelPen(s.labelColor);
	QFont labelFont(s.labelFont);
	painter->setPen(labelPen);
	painter->setFont(labelFont);

	for (int i = 0; i < cols.size(); ++i) {
		const auto& c = cols.at(i);
		QString caption;
		auto it = cs->valueCaptions.find(c.value.value());
		if (it != cs->valueCaptions.end()) {
			caption = it->second;
		}
		auto rect = metrics.boundingRect(caption);
		double left = barLeftStart + barWidth * i;

		QRectF textRect(left, top, barWidth, rect.height());
		painter->drawText(textRect, Qt::AlignHCenter | Qt::AlignVCenter, caption);
	}
}
