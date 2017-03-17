#include "crosssection.h"
#include "crosssectionpreprocessorviewhelper.h"
#include "../base/model.h"
#include "../base/view.h"

#include <QPainter>

namespace {

const int pointSize = 5;
const int fontSize = 10;

void drawPoint(const QPointF& p, const QColor& c, QPainter* painter)
{
	QRectF rect(p.x() - pointSize * 0.5, p.y() - pointSize * 0.5, pointSize, pointSize);
	painter->fillRect(rect, c);
}

}

CrossSectionPreProcessorViewHelper::CrossSectionPreProcessorViewHelper(DataItemView* v) :
	DataItemViewHelperI {v}
{}

void CrossSectionPreProcessorViewHelper::draw(QPainter *painter) const
{
	QColor color(60, 60, 0);

	auto cs = dynamic_cast<CrossSection*> (dataItem());
	if (! cs->isDefined()) {return;}

	painter->save();
	auto v = view();

	QPen pen(color);
	if (model()->isSelected(cs)) {
		pen.setWidthF(2);
	}
	painter->setPen(pen);

	painter->drawLine(v->conv(cs->point1()), v->conv(cs->point2()));

	if (model()->isSelected(cs)) {
		drawPoint(v->conv(cs->point1()), color, painter);
		drawPoint(v->conv(cs->point2()), color, painter);
	}

	drawPoint(v->conv(cs->crossSectionPoint()), color, painter);

	painter->restore();
}
