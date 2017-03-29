#include "crosssection.h"
#include "crosssectionpreprocessorviewlabelhelper.h"
#include "../base/view.h"

#include <QFont>
#include <QPainter>

namespace {

int labelOffset = 8;
int fontSize = 10;

} // namespace

CrossSectionPreProcessorViewLabelHelper::CrossSectionPreProcessorViewLabelHelper(DataItemView* v) :
	DataItemViewHelperI {v}
{}

void CrossSectionPreProcessorViewLabelHelper::draw(QPainter *painter) const
{
	QColor color(60, 60, 0);

	auto cs = dynamic_cast<CrossSection*> (dataItem());
	if (! cs->isDefined()) {return;}

	painter->save();
	auto v = view();

	QFont f;
	f.setPointSize(fontSize);
	painter->setPen(color);
	painter->setFont(f);

	QPointF p_lb = v->conv(cs->point1());
	p_lb += QPointF(labelOffset, fontSize * 0.5);
	painter->drawText(p_rb, tr("%1 LB").arg(cs->name()));

	QPointF p_rb = v->conv(cs->point2());
	p_rb += QPointF(labelOffset, fontSize * 0.5);
	painter->drawText(p_rb, tr("%1 RB").arg(cs->name()));

	painter->restore();
}
