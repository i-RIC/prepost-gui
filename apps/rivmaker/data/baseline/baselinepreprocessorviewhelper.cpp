#include "baseline.h"
#include "baselinepreprocessorviewhelper.h"
#include "../base/model.h"
#include "../base/view.h"

#include <QPainter>
#include <QPen>

BaseLinePreProcessorViewHelper::BaseLinePreProcessorViewHelper(DataItemView* v) :
	DataItemViewHelperI {v}
{}

void BaseLinePreProcessorViewHelper::draw(QPainter* painter) const
{
	auto baseLine = dynamic_cast<BaseLine*> (dataItem());
	if (baseLine->polyLine().size() < 2) {return;}

	painter->save();
	auto v = view();

	QPen pen(Qt::blue);
	if (model()->isSelected(dataItem())) {
		pen.setWidthF(3);
	}
	painter->setPen(pen);

	const auto& polyLine = baseLine->polyLine();
	for (int i = 0; i < polyLine.size() - 1; ++i) {
		QPointF startP = polyLine.at(i);
		QPointF endP = polyLine.at(i + 1);
		painter->drawLine(v->conv(startP), v->conv(endP));
	}

	painter->restore();
}
