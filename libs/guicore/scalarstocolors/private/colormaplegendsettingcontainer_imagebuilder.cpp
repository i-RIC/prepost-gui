#include "../colormapsettingcontainer.h"
#include "colormaplegendsettingcontainer_imagebuilder.h"

#include <misc/stringtool.h>

#include <QLinearGradient>

namespace {

int OUTER_MARGIN = 12;
int INNER_MARGIN = 10;
int COLOR_VALUE_MARGIN = 10;
int LABEL_MARGIN = 5;
int STD_COLOR_WIDTH = 30;

} // namespace

ColorMapLegendSettingContainer::ImageBuilder::ImageBuilder(ColorMapLegendSettingContainer* setting) :
	m_setting {setting}
{}

bool ColorMapLegendSettingContainer::ImageBuilder::build(QImage* image)
{
	ColorMapLegendSettingContainer s;
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
	double opacity = s.backgroundOpacity;
	QBrush brush(QColor(bgCol.red(), bgCol.green(), bgCol.blue(), static_cast<int>(255 * opacity)));
	painter.setPen(pen);
	painter.setBrush(brush);
	painter.drawRect(0, 0, (image->width() / image->devicePixelRatioF() - 1), (image->height() / image->devicePixelRatioF() - 1));

	if (s.direction == Direction::Vertical) {
		buildVertical(&painter, image, s);
	} else if (s.direction == Direction::Horizontal) {
		buildHorizontal(&painter, image, s);
	}
	return true;
}

QSize ColorMapLegendSettingContainer::ImageBuilder::autoSize() const
{
	int width, height;

	ColorMapLegendSettingContainer s;
	if (m_setting->delegateMode()) {
		s = m_setting->colorMapSetting()->legend;
		s.visible = m_setting->visible;
	} else {
		s = *m_setting;
	}
	s.setColorMapSetting(m_setting->colorMapSetting());

	if (s.direction == Direction::Vertical) {
		QFontMetrics titleMetrics(s.titleFont);
		int title_width = titleMetrics.width(s.title);
		int value_width = 0;

		auto cs = s.colorMapSetting();
		auto cols = cs->getColors();
		auto f = iRIC::toStr(s.labelFormat.value());
		QFontMetrics labelMetrics(s.labelFont);
		for (const auto& c : cols) {
			QString val = QString::asprintf(f.c_str(), c.value.value());
			value_width = std::max(value_width, labelMetrics.width(val));
		}
		value_width += LABEL_MARGIN + COLOR_VALUE_MARGIN;
		if (s.barAutoWidth) {
			value_width += STD_COLOR_WIDTH;
		} else {
			value_width += s.barWidth;
		}

		int content_width = std::max(title_width, value_width);
		width = content_width + OUTER_MARGIN * 2;
		if (s.colorMapSetting()->transitionMode == ColorMapSettingContainer::TransitionMode::Continuous) {
			height = OUTER_MARGIN * 2 + INNER_MARGIN + titleMetrics.height() + (labelMetrics.height() + 2) * cols.size();
		} else {
			height = OUTER_MARGIN * 2 + INNER_MARGIN + titleMetrics.height() + (labelMetrics.height() + 2) * (cols.size() + 1);
		}
	} else if (s.direction == Direction::Horizontal) {
		QFontMetrics titleMetrics(s.titleFont);
		int title_width = titleMetrics.width(s.title);

		int value_width = 0;

		auto cs = s.colorMapSetting();
		auto cols = cs->getColors();
		auto f = iRIC::toStr(s.labelFormat.value());
		QFontMetrics labelMetrics(s.labelFont);
		for (const auto& c : cols) {
			QString val = QString::asprintf(f.c_str(), c.value.value());
			value_width = std::max(value_width, labelMetrics.width(val));
		}
		int value_total_width = (value_width + LABEL_MARGIN) * cols.size();

		int content_width = std::max(title_width, value_total_width);
		width = content_width + OUTER_MARGIN * 2;
		height = OUTER_MARGIN * 2 + INNER_MARGIN + titleMetrics.height() + labelMetrics.height() + s.barWidth;
	}

	return QSize(width, height);
}

void ColorMapLegendSettingContainer::ImageBuilder::buildVertical(QPainter* painter, QImage* image, const ColorMapLegendSettingContainer& s)
{
	painter->setPen(s.titleColor);
	painter->setFont(s.titleFont);
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

	painter->drawText(rect, align, s.title);

	int bar_top = (OUTER_MARGIN + painter->fontMetrics().height() + INNER_MARGIN);
	int bar_bottom = (image->height() / image->devicePixelRatioF() - OUTER_MARGIN);

	if (s.m_colorMapSetting->transitionMode == ColorMapSettingContainer::TransitionMode::Continuous) {
		buildContinuousVertical(painter, bar_top, bar_bottom, image, s);
	} else {
		buildDiscreteVertical(painter, bar_top, bar_bottom, image, s);
	}
}

void ColorMapLegendSettingContainer::ImageBuilder::buildContinuousVertical(QPainter* painter, int top, int bottom, QImage* image, const ColorMapLegendSettingContainer& s)
{
	int maxLabelWidth = 0;

	auto cs = s.m_colorMapSetting;

	QFontMetrics metrics(s.labelFont);

	auto cols = cs->getColors();
	auto f = iRIC::toStr(s.labelFormat.value());
	for (const auto& c : cols) {
		QString val = QString::asprintf(f.c_str(), c.value.value());
		auto rect = metrics.boundingRect(val);
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

	double barHeight = (bottom - top) / static_cast<double>(cols.size() - 1);

	// colors
	double barLeft = 0;
	if (s.barAlign == BarAlign::Left) {
		barLeft = OUTER_MARGIN;
	} else if (s.barAlign == BarAlign::Center) {
		barLeft = image->width() / image->devicePixelRatioF() / 2.0 - (maxLabelWidth + COLOR_VALUE_MARGIN + colorWidth) / 2.0;
	} else if (s.barAlign == BarAlign::Right) {
		barLeft = image->width() / image->devicePixelRatioF() - OUTER_MARGIN - maxLabelWidth - COLOR_VALUE_MARGIN - colorWidth;
	}
	for (int i = 0; i < cols.size() - 1; ++i) {
		auto colStart = cols.at(i).color;
		auto colEnd = cols.at(i + 1).color;

		double barBottom = image->height() / image->devicePixelRatioF() - OUTER_MARGIN - barHeight * i;
		QLinearGradient gradient(QPointF(0, barBottom), QPointF(0, barBottom - barHeight));
		gradient.setColorAt(0, colStart);
		gradient.setColorAt(1, colEnd);
		QBrush brush(gradient);

		QRectF rect(barLeft, barBottom - barHeight, colorWidth, barHeight);
		painter->fillRect(rect, brush);
	}

	// labels
	double labelLeft = barLeft + colorWidth + COLOR_VALUE_MARGIN;
	double labelWidth = image->width() / image->devicePixelRatioF() - labelLeft - OUTER_MARGIN;
	if (labelWidth < 0) {labelWidth = 0;}

	QPen labelPen(s.labelColor);
	QFont labelFont(s.labelFont);
	painter->setPen(labelPen);
	painter->setFont(labelFont);

	if (s.autoNumberOfLabels) {
		for (int i = 0; i < cols.size(); ++i) {
			QString val = QString::asprintf(f.c_str(), cols.at(i).value.value());
			auto rect = metrics.boundingRect(val);

			double bottom = image->height() / image->devicePixelRatioF() - OUTER_MARGIN - barHeight * i;

			QRectF textRect(labelLeft, bottom - rect.height() * 0.5, labelWidth, rect.height());
			painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, val);
		}
	} else {
		auto colors = s.colorMapSetting()->getColors();
		double origStep = 1.0 / (colors.size() - 1);
		double step = 1.0 / (s.numberOfLabels.value() - 1);
		int labelHeight = (bottom - top) / (s.numberOfLabels - 1);
		for (int i = 0; i < s.numberOfLabels; ++i) {
			double v = i * step;
			auto id1 = static_cast<unsigned int> (v / origStep);
			double r = (v - (id1 * origStep)) / origStep;

			double value;
			if (id1 == colors.size() - 1) {
				value = colors.at(colors.size() - 1).value;
			} else {
				double v1 = colors.at(id1).value;
				double v2 = colors.at(id1 + 1).value;
				value = v1 * (1 - r) + v2 * r;
			}
			QString val = QString::asprintf(f.c_str(), value);
			auto rect = metrics.boundingRect(val);

			int bottom = image->height() / image->devicePixelRatioF() - OUTER_MARGIN - labelHeight * i;

			QRectF textRect(labelLeft, bottom - rect.height() * 0.5, labelWidth, rect.height());
			painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, val);
		}
	}
}

void ColorMapLegendSettingContainer::ImageBuilder::buildDiscreteVertical(QPainter* painter, int top, int bottom, QImage* image, const ColorMapLegendSettingContainer& s)
{
	int maxLabelWidth = 0;

	auto cs = s.m_colorMapSetting;

	QFontMetrics metrics(s.labelFont);

	auto cols = cs->getColors();
	auto f = iRIC::toStr(s.labelFormat.value());

	for (const auto& c : cols) {
		QString val = QString::asprintf(f.c_str(), c.value.value());
		auto rect = metrics.boundingRect(val);
		if (rect.width() > maxLabelWidth) {
			maxLabelWidth = rect.width();
		}
	}
	QString val = QString::asprintf(f.c_str(), cs->getColorTableMinValue());
	auto rect = metrics.boundingRect(val);
	if (rect.width() > maxLabelWidth) {
		maxLabelWidth = rect.width();
	}
	maxLabelWidth += LABEL_MARGIN;

	int colorWidth = 0;
	if (s.barAutoWidth) {
		colorWidth = image->width() / image->devicePixelRatioF() - OUTER_MARGIN * 2 - COLOR_VALUE_MARGIN - maxLabelWidth;
	} else {
		colorWidth = s.barWidth;
	}
	double barHeight = (bottom - top) / static_cast<double>(cols.size());

	// colors
	double barLeft = 0;
	if (s.barAlign == BarAlign::Left) {
		barLeft = OUTER_MARGIN;
	} else if (s.barAlign == BarAlign::Center) {
		barLeft = image->width() / image->devicePixelRatioF() / 2.0 - (maxLabelWidth + COLOR_VALUE_MARGIN + colorWidth) / 2.0;
	} else if (s.barAlign == BarAlign::Right) {
		barLeft = image->width() / image->devicePixelRatioF() - OUTER_MARGIN - maxLabelWidth - COLOR_VALUE_MARGIN - colorWidth;
	}
	for (int i = 0; i < cols.size(); ++i) {
		auto c = cols.at(i);
		auto col = c.color;

		double barBottom = image->height() / image->devicePixelRatioF() - OUTER_MARGIN - barHeight * i;
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

	QString val2 = QString::asprintf(f.c_str(), cs->getColorTableMinValue());
	auto rect2 = metrics.boundingRect(val2);
	int bottom2 = image->height() / image->devicePixelRatioF() - OUTER_MARGIN;

	QRectF textRect(labelLeft, bottom2 - rect2.height() * 0.5, labelWidth, rect2.height());
	painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, val2);

	for (int i = 0; i < cols.size(); ++i) {
		if ((i + 1) % s.labelSkipRate != 0) {continue;}

		QString val = QString::asprintf(f.c_str(), cols.at(i).value.value());
		auto rect = metrics.boundingRect(val);

		double bottom = image->height() / image->devicePixelRatioF() - OUTER_MARGIN - barHeight * (i + 1);

		QRectF textRect(labelLeft, bottom - rect.height() * 0.5, labelWidth, rect.height());
		painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, val);
	}
}

void ColorMapLegendSettingContainer::ImageBuilder::buildHorizontal(QPainter* painter, QImage *image, const ColorMapLegendSettingContainer& s)
{
	painter->setPen(s.titleColor);
	painter->setFont(s.titleFont);
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

	painter->drawText(rect, align, s.title);

	int bar_top = (OUTER_MARGIN + painter->fontMetrics().height() + INNER_MARGIN);
	int bar_bottom = (image->height() / image->devicePixelRatioF() - OUTER_MARGIN);

	if (s.m_colorMapSetting->transitionMode == ColorMapSettingContainer::TransitionMode::Continuous) {
		buildContinuousHorizontal(painter, bar_top, bar_bottom, image, s);
	} else {
		buildDiscreteHorizontal(painter, bar_top, bar_bottom, image, s);
	}
}

void ColorMapLegendSettingContainer::ImageBuilder::buildContinuousHorizontal(QPainter* painter, int top, int bottom, QImage* image, const ColorMapLegendSettingContainer& s)
{
	double maxLabelWidth = 0;
	int labelHeight = 0;

	auto cs = s.m_colorMapSetting;

	QFontMetrics metrics(s.labelFont);

	auto cols = cs->getColors();
	auto f = iRIC::toStr(s.labelFormat.value());
	for (const auto& c : cols) {
		QString val = QString::asprintf(f.c_str(), c.value.value());
		auto rect = metrics.boundingRect(val);
		if (rect.width() > maxLabelWidth) {
			maxLabelWidth = rect.width() + 5;
		}

		labelHeight = rect.height();
	}

	int colorHeight = (bottom - top) - labelHeight - LABEL_MARGIN;
	double barWidth = (image->width() / image->devicePixelRatioF() - OUTER_MARGIN * 2 - maxLabelWidth) / static_cast<double>(cols.size() - 1);

	// colors
	double barLeftStart = 0;
	if (s.barAlign == BarAlign::Left) {
		barLeftStart = OUTER_MARGIN + maxLabelWidth / 2;
	} else if (s.barAlign == BarAlign::Center) {
		barLeftStart = image->width() / image->devicePixelRatioF() / 2 - (barWidth * (cols.size() - 1)) / 2.0;
	} else if (s.barAlign == BarAlign::Right) {
		barLeftStart = image->width() / image->devicePixelRatioF() - OUTER_MARGIN - maxLabelWidth / 2 - (barWidth * (cols.size() - 1));
	}
	for (int i = 0; i < cols.size() - 1; ++i) {
		auto colStart = cols.at(i).color;
		auto colEnd = cols.at(i + 1).color;

		double barLeft = barLeftStart + barWidth * i;
		QLinearGradient gradient(QPointF(barLeft, 0), QPointF(barLeft + barWidth, 0));
		gradient.setColorAt(0, colStart);
		gradient.setColorAt(1, colEnd);
		QBrush brush(gradient);

		QRectF rect(barLeft, top + labelHeight + LABEL_MARGIN, barWidth, colorHeight);
		painter->fillRect(rect, brush);
	}

	// labels
	QPen labelPen(s.labelColor);
	QFont labelFont(s.labelFont);
	painter->setPen(labelPen);
	painter->setFont(labelFont);

	if (s.autoNumberOfLabels) {
		for (int i = 0; i < cols.size(); ++i) {
			QString val = QString::asprintf(f.c_str(), cols.at(i).value.value());
			auto rect = metrics.boundingRect(val);
			double left = barLeftStart - maxLabelWidth / 2 + barWidth * i;

			QRectF textRect(left, top, maxLabelWidth, rect.height());
			painter->drawText(textRect, Qt::AlignCenter | Qt::AlignVCenter, val);
		}
	} else {
		auto colors = s.colorMapSetting()->getColors();
		double origStep = 1.0 / (colors.size() - 1);
		double step = 1.0 / (s.numberOfLabels.value() - 1);
		double labelWidth = (image->width() / image->devicePixelRatioF() - OUTER_MARGIN * 2 - maxLabelWidth) / static_cast<double>(s.numberOfLabels.value() - 1);

		for (int i = 0; i < s.numberOfLabels; ++i) {
			double v = i * step;
			auto id1 = static_cast<unsigned int> (v / origStep);
			double r = (v - (id1 * origStep)) / origStep;

			double value;
			if (id1 == colors.size() - 1) {
				value = colors.at(colors.size() - 1).value;
			} else {
				double v1 = colors.at(id1).value;
				double v2 = colors.at(id1 + 1).value;
				value = v1 * (1 - r) + v2 * r;
			}
			QString val = QString::asprintf(f.c_str(), value);
			auto rect = metrics.boundingRect(val);
			double left = barLeftStart - maxLabelWidth / 2 + labelWidth * i;

			QRectF textRect(left, top, maxLabelWidth, rect.height() + 2);
			painter->drawText(textRect, Qt::AlignCenter | Qt::AlignVCenter, val);
		}
	}
}

void ColorMapLegendSettingContainer::ImageBuilder::buildDiscreteHorizontal(QPainter* painter, int top, int bottom, QImage* image, const ColorMapLegendSettingContainer& s)
{
	int maxLabelWidth = 0;
	int labelHeight = 0;

	auto cs = s.m_colorMapSetting;

	QFontMetrics metrics(s.labelFont);

	auto cols = cs->getColors();
	auto f = iRIC::toStr(s.labelFormat.value());
	for (const auto& c : cols) {
		QString val = QString::asprintf(f.c_str(), c.value.value());
		auto rect = metrics.boundingRect(val);
		if (rect.width() > maxLabelWidth) {
			maxLabelWidth = rect.width();
		}

		labelHeight = rect.height();
	}

	int colorHeight = (bottom - top) - labelHeight - LABEL_MARGIN;
	double barWidth = (image->width() / image->devicePixelRatioF() - OUTER_MARGIN * 2 - maxLabelWidth) / static_cast<double>(cols.size());

	// colors
	double barLeftStart = 0;
	if (s.barAlign == BarAlign::Left) {
		barLeftStart = OUTER_MARGIN + maxLabelWidth / 2;
	} else if (s.barAlign == BarAlign::Center) {
		barLeftStart = image->width() / image->devicePixelRatioF() / 2 - (barWidth * cols.size()) / 2.0;
	} else if (s.barAlign == BarAlign::Right) {
		barLeftStart = image->width() / image->devicePixelRatioF() - OUTER_MARGIN - maxLabelWidth / 2 - (barWidth * cols.size());
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

	QString val2 = QString::asprintf(f.c_str(), cs->getColorTableMinValue());
	auto rect2 = metrics.boundingRect(val2);

	QRectF textRect(barLeftStart - maxLabelWidth / 2, top, maxLabelWidth, rect2.height());
	painter->drawText(textRect, Qt::AlignCenter | Qt::AlignVCenter, val2);

	for (int i = 0; i < cols.size(); ++i) {
		if ((i + 1) % s.labelSkipRate != 0) {continue;}

		QString val = QString::asprintf(f.c_str(), cols.at(i).value.value());
		auto rect = metrics.boundingRect(val);
		double left = barLeftStart - maxLabelWidth / 2 + barWidth * (i + 1);

		QRectF textRect(left, top, maxLabelWidth, rect.height());
		painter->drawText(textRect, Qt::AlignCenter | Qt::AlignVCenter, val);
	}
}
