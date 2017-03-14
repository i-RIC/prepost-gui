#include "baseline.h"
#include "baselinepreprocessorviewhelper.h"
#include "../base/model.h"
#include "../base/view.h"

#include <QPainter>
#include <QPen>

namespace {

const int pointSize = 5;

}

BaseLinePreProcessorViewHelper::BaseLinePreProcessorViewHelper(DataItemView* v) :
	DataItemViewHelperI {v}
{}

void BaseLinePreProcessorViewHelper::draw(QPainter* painter) const
{
	QColor baseLineColor(60, 60, 60);

	auto baseLine = dynamic_cast<BaseLine*> (dataItem());
	if (baseLine->polyLine().size() < 2) {return;}

	painter->save();
	auto v = view();

	QPen pen(baseLineColor);
	if (model()->isSelected(dataItem())) {
		pen.setWidthF(2);
	}
	painter->setPen(pen);

	const auto& polyLine = baseLine->polyLine();
	for (int i = 0; i < polyLine.size() - 1; ++i) {
		QPointF startP = polyLine.at(i);
		QPointF endP = polyLine.at(i + 1);
		painter->drawLine(v->conv(startP), v->conv(endP));
	}

	if (model()->isSelected(dataItem())) {
		for (QPointF p : polyLine) {
			QPointF p2 = v->conv(p);
			QRectF rect(p2.x() - pointSize * 0.5, p2.y() - pointSize * 0.5, pointSize, pointSize);

			painter->fillRect(rect, baseLineColor);
		}
	}

	painter->restore();
}
