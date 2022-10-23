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
	painter.drawRect(0, 0, image->width() / image->devicePixelRatioF() - 1, image->height() / image->devicePixelRatioF() - 1);

	painter.setPen(s->titleColor);
	painter.setFont(s->titleFont);
	QFontMetrics metrics(s->titleFont);

	QRect rect(OUTER_MARGIN, OUTER_MARGIN, image->width() / image->devicePixelRatioF() - OUTER_MARGIN * 2, metrics.height());
	painter.drawText(rect, Qt::AlignHCenter, s->title);

	int bar_top = OUTER_MARGIN + painter.fontMetrics().height() + INNER_MARGIN;
	int bar_bottom = image->height() / image->devicePixelRatioF() - OUTER_MARGIN;

	buildDiscrete(&painter, bar_top, bar_bottom, image);

	return true;
}

void ColorMapEnumerateLegendSettingContainer::ImageBuilder::buildDiscrete(QPainter* painter, int top, int bottom, QImage* image)
{
	int maxLabelWidth = 0;

	auto s = m_setting;
	auto cs = s->m_colorMapSetting;
	if (cs->colors.size() == 0) {return;}

	QFontMetrics metrics(s->labelFont);

	const auto& cols = cs->colors;

	for (const auto& c : cols) {
		auto val = cs->valueCaptions.at(c.value.value());
		auto rect = metrics.boundingRect(val);
		if (rect.width() > maxLabelWidth) {
			maxLabelWidth = rect.width();
		}
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

	for (int i = 0; i < cols.size(); ++i) {
		const auto& c = cols.at(i);
		auto val = cs->valueCaptions.at(c.value.value());
		auto rect = metrics.boundingRect(val);

		int bottom = image->height() / image->devicePixelRatioF() - OUTER_MARGIN - barHeight * (i + 0.5);

		QRectF textRect(left, bottom - rect.height() * 0.5, labelWidth, rect.height());
		painter->drawText(textRect, val);
	}
}
