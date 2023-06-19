#include "../arrowssettingcontainer.h"
#include "arrowslegendsettingcontainer_imagebuilder.h"

#include <misc/stringtool.h>

namespace {

int OUTER_MARGIN = 12;
int INNER_MARGIN = 10;

} // namespace

ArrowsLegendSettingContainer::ImageBuilder::ImageBuilder(ArrowsLegendSettingContainer* setting) :
	m_setting {setting}
{}

bool ArrowsLegendSettingContainer::ImageBuilder::build(QImage* image)
{
	if (! m_setting->visible) {return false;}

	const auto& s = m_setting;
	const auto& as = s->m_arrowsSetting;

	if (as->target.value().isEmpty()) {return false;}

	QPainter painter(image);

	auto bgCol = s->backgroundColor.value();
	QPen pen;
	if (s->showBorder) {
		pen = QPen(bgCol);
	} else {
		pen = QPen(Qt::transparent);
	}
	double opacity = s->backgroundOpacity;
	QBrush brush(QColor(bgCol.red(), bgCol.green(), bgCol.blue(), static_cast<int>(255 * opacity)));
	painter.setPen(pen);
	painter.setBrush(brush);
	painter.drawRect(0, 0, image->width() / image->devicePixelRatioF() - 1, image->height() / image->devicePixelRatioF() - 1);

	painter.setPen(s->titleColor);
	painter.setFont(s->titleFont);
	QFontMetrics metrics(s->titleFont);

	QRect rect(OUTER_MARGIN, OUTER_MARGIN, image->width() / image->devicePixelRatioF() - OUTER_MARGIN * 2, metrics.height());
	painter.drawText(rect, Qt::AlignHCenter, s->title);

	painter.setPen(s->lengthColor);
	painter.setFont(s->lengthFont);
	QFontMetrics metrics2(s->lengthFont);

	QRect rect2(OUTER_MARGIN, OUTER_MARGIN + INNER_MARGIN * 2 + metrics.height() + as->lineWidth.value(),
							image->width() / image->devicePixelRatioF() - OUTER_MARGIN * 2, metrics2.height());

	auto format = iRIC::toStr(s->lengthFormat);
	QString val = QString::asprintf(format.c_str(), as->standardValue.value());
	painter.drawText(rect2, Qt::AlignHCenter, val);

	QColor arrowColor = Qt::black;
	if (as->colorMode == ArrowsSettingContainer::ColorMode::Custom) {
		arrowColor = as->customColor;
	}
	QPen linePen(arrowColor, as->lineWidth.value());
	painter.setPen(linePen);

	double y = OUTER_MARGIN + metrics.height() + INNER_MARGIN + as->lineWidth.value() * 0.5;
	painter.drawLine(QPointF(image->width() / image->devicePixelRatioF() * 0.5 - as->legendLength * 0.5, y), QPointF(image->width() / image->devicePixelRatioF() * 0.5 + as->legendLength * 0.5, y));

	double dy = as->arrowSize * 0.3;
	QPolygonF pol;
	pol.append(QPointF(image->width() / image->devicePixelRatioF() * 0.5 + as->legendLength * 0.5, y));
	pol.append(QPointF(image->width() / image->devicePixelRatioF() * 0.5 + as->legendLength * 0.5 - as->arrowSize, y - dy));
	pol.append(QPointF(image->width() / image->devicePixelRatioF() * 0.5 + as->legendLength * 0.5 - as->arrowSize, y + dy));
	pol.append(QPointF(image->width() / image->devicePixelRatioF() * 0.5 + as->legendLength * 0.5, y));
	painter.setPen(arrowColor);
	painter.setBrush(QBrush(arrowColor));
	painter.drawPolygon(pol);

	return true;
}
