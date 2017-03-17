#include "baseline.h"
#include "baselinepreprocessorviewlabelhelper.h"
#include "../base/model.h"
#include "../base/view.h"

#include <QPainter>

namespace {

int labelOffset = 8;
int fontSize = 10;

} // namespace

BaseLinePreProcessorViewLabelHelper::BaseLinePreProcessorViewLabelHelper(DataItemView* v) :
	DataItemViewHelperI {v}
{}

void BaseLinePreProcessorViewLabelHelper::draw(QPainter* painter) const
{
	QColor color(Qt::black);

	auto baseLine = dynamic_cast<BaseLine*> (dataItem());
	if (baseLine->polyLine().size() < 2) {return;}

	painter->save();
	auto v = view();

	QFont f;
	f.setPointSize(fontSize);
	painter->setPen(color);
	painter->setFont(f);

	const auto& line = baseLine->polyLine();

	// downstream
	QPointF pd = v->conv(line.at(0));
	pd += QPointF(labelOffset, fontSize * 0.5);
	painter->drawText(pd, tr("Downstream"));

	// upstream
	QPointF pu = v->conv(line.at(line.size() - 1));
	pu += QPointF(labelOffset, fontSize * 0.5);
	painter->drawText(pu, tr("Upstream"));

	painter->restore();
}
