#include "../colormapsettingcontainer.h"
#include "colormaplegendsettingcontainer_imagebuilder.h"

#include <misc/stringtool.h>

#include <QLinearGradient>

namespace {

int OUTER_MARGIN = 12;
int INNER_MARGIN = 10;
int COLOR_VALUE_MARGIN = 10;
int LABEL_MARGIN = 5;

} // namespace

ColorMapLegendSettingContainer::ImageBuilder::ImageBuilder(ColorMapLegendSettingContainer* setting) :
	m_setting {setting}
{}

bool ColorMapLegendSettingContainer::ImageBuilder::build(QImage* image)
{
	auto vmode = m_setting->visibilityMode.value();

	if (vmode == VisibilityMode::Never) {return false;}
	if (vmode == VisibilityMode::WhenSelected && ! m_setting->imageSetting.controller()->selected()) {return false;}

	const auto& s = m_setting;

	QPainter painter(image);

	auto bgCol = s->backgroundColor.value();
	QPen pen(bgCol);
	double opacity = s->backgroundOpacity.value() / 100.0;
	QBrush brush(QColor(bgCol.red(), bgCol.green(), bgCol.blue(), static_cast<int>(255 * opacity)));
	painter.setPen(pen);
	painter.setBrush(brush);
	painter.drawRect(0, 0, (image->width() / image->devicePixelRatioF() - 1), (image->height() / image->devicePixelRatioF() - 1));

	painter.setPen(s->titleColor);
	painter.setFont(s->titleFont);
	QFontMetrics metrics(s->titleFont);

	QRect rect(OUTER_MARGIN, OUTER_MARGIN, image->width() / image->devicePixelRatioF() - OUTER_MARGIN * 2, metrics.height());
	painter.drawText(rect, Qt::AlignHCenter, s->title);

	int bar_top = (OUTER_MARGIN + painter.fontMetrics().height() + INNER_MARGIN);
	int bar_bottom = (image->height() / image->devicePixelRatioF() - OUTER_MARGIN);

	if (s->m_colorMapSetting->transitionMode == ColorMapSettingContainer::TransitionMode::Continuous) {
		buildContinuous(&painter, bar_top, bar_bottom, image);
	} else {
		buildDiscrete(&painter, bar_top, bar_bottom, image);
	}

	return true;
}

void ColorMapLegendSettingContainer::ImageBuilder::buildContinuous(QPainter* painter, int top, int bottom, QImage* image)
{
	int maxLabelWidth = 0;

	auto s = m_setting;
	auto cs = s->m_colorMapSetting;

	QFontMetrics metrics(s->labelFont);

	auto cols = cs->getColors();
	auto f = iRIC::toStr(s->labelFormat.value());
	for (const auto& c : cols) {
		QString val = QString::asprintf(f.c_str(), c.value.value());
		auto rect = metrics.boundingRect(val);
		if (rect.width() > maxLabelWidth) {
			maxLabelWidth = rect.width();
		}
	}
	maxLabelWidth += LABEL_MARGIN;

	int colorWidth = image->width() / image->devicePixelRatioF() - OUTER_MARGIN * 2 - COLOR_VALUE_MARGIN - maxLabelWidth;
	int barHeight = (bottom - top) / (cols.size() - 1);

	// colors
	for (int i = 0; i < cols.size() - 1; ++i) {
		auto colStart = cols.at(i).color;
		auto colEnd = cols.at(i + 1).color;

		int barBottom = image->height() / image->devicePixelRatioF() - OUTER_MARGIN - barHeight * i;
		QLinearGradient gradient(QPoint(0, barBottom), QPoint(0, barBottom - barHeight));
		gradient.setColorAt(0, colStart);
		gradient.setColorAt(1, colEnd);
		QBrush brush(gradient);

		QRectF rect(OUTER_MARGIN, barBottom - barHeight, colorWidth, barHeight);
		painter->fillRect(rect, brush);
	}

	// labels
	int left = OUTER_MARGIN + colorWidth + COLOR_VALUE_MARGIN;
	int labelWidth = image->width() / image->devicePixelRatioF() - left - OUTER_MARGIN;
	if (labelWidth < 0) {labelWidth = 0;}

	QPen labelPen(s->labelColor);
	QFont labelFont(s->labelFont);
	painter->setPen(labelPen);
	painter->setFont(labelFont);

	if (s->autoNumberOfLabels) {
		for (int i = 0; i < cols.size(); ++i) {
			QString val = QString::asprintf(f.c_str(), cols.at(i).value.value());
			auto rect = metrics.boundingRect(val);

			int bottom = image->height() / image->devicePixelRatioF() - OUTER_MARGIN - barHeight * i;

			QRectF textRect(left, bottom - rect.height() * 0.5, labelWidth, rect.height());
			painter->drawText(textRect, val);
		}
	} else {
		auto colors = s->colorMapSetting()->getColors();
		double origStep = 1.0 / (colors.size() - 1);
		double step = 1.0 / (s->numberOfLabels.value() - 1);
		int labelHeight = (bottom - top) / (s->numberOfLabels - 1);
		for (int i = 0; i < s->numberOfLabels; ++i) {
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

			QRectF textRect(left, bottom - rect.height() * 0.5, labelWidth, rect.height());
			painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, val);
		}
	}
}

void ColorMapLegendSettingContainer::ImageBuilder::buildDiscrete(QPainter* painter, int top, int bottom, QImage* image)
{
	int maxLabelWidth = 0;

	auto s = m_setting;
	auto cs = s->m_colorMapSetting;

	QFontMetrics metrics(s->labelFont);

	auto cols = cs->getColors();
	auto f = iRIC::toStr(s->labelFormat.value());

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

	int colorWidth = image->width() / image->devicePixelRatioF() - OUTER_MARGIN * 2 - COLOR_VALUE_MARGIN - maxLabelWidth;
	int barHeight = (bottom - top) / (cols.size());

	// colors
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

			QRectF rect(OUTER_MARGIN + 1, barBottom - barHeight + 1, colorWidth - 2, barHeight - 2);
			painter->drawRect(rect);
		} else {

			QRectF rect(OUTER_MARGIN, barBottom - barHeight, colorWidth, barHeight);
			painter->fillRect(rect, brush);
		}
	}

	// labels
	int left = OUTER_MARGIN + colorWidth + COLOR_VALUE_MARGIN;
	int labelWidth = image->width() - left - OUTER_MARGIN;
	if (labelWidth < 0) {labelWidth = 0;}

	QPen labelPen(s->labelColor);
	QFont labelFont(s->labelFont);
	painter->setPen(labelPen);
	painter->setFont(labelFont);

	QString val2 = QString::asprintf(f.c_str(), cs->getColorTableMinValue());
	auto rect2 = metrics.boundingRect(val2);
	int bottom2 = image->height() - OUTER_MARGIN;

	QRectF textRect(left, bottom2 - rect2.height() * 0.5, labelWidth, rect2.height());
	painter->drawText(textRect, val2);

	for (int i = 0; i < cols.size(); ++i) {
		QString val = QString::asprintf(f.c_str(), cols.at(i).value.value());
		auto rect = metrics.boundingRect(val);

		int bottom = image->height() - OUTER_MARGIN - barHeight * (i + 1);

		QRectF textRect(left, bottom - rect.height() * 0.5, labelWidth, rect.height());
		painter->drawText(textRect, val);
	}
}
