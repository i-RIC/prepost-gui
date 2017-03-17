#include "crosssection.h"
#include "crosssectionpreprocessorviewlabelhelper.h"
#include "../base/view.h"

#include <QFont>
#include <QPainter>

namespace {

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

	QPointF p2 = v->conv(cs->point2());

	QFont f;
	f.setPointSize(fontSize);
	painter->setPen(color);
	painter->setFont(f);
	painter->drawText(p2, cs->name());

	painter->restore();
}
