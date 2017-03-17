#include "baseline.h"
#include "baselinepreprocessorview.h"
#include "baselinepreprocessorviewhelper.h"
#include "baselinepreprocessorviewlabelhelper.h"

#include <QRectF>

BaseLinePreProcessorView::BaseLinePreProcessorView(Model* model, BaseLine* item) :
	DataItemView {model, item}
{
	viewHelpers().push_back(new BaseLinePreProcessorViewHelper(this));
	viewHelpers().push_back(new BaseLinePreProcessorViewLabelHelper(this));
}

BaseLinePreProcessorView::~BaseLinePreProcessorView()
{}

QRectF BaseLinePreProcessorView::doBoundingBox() const
{
	bool first = true;
	double xmin, xmax, ymin, ymax;

	auto bline = dynamic_cast<BaseLine*> (item());
	for (QPointF p : bline->polyLine()) {
		if (first || p.x() < xmin) {xmin = p.x();}
		if (first || p.x() > xmax) {xmax = p.x();}
		if (first || p.y() < ymin) {ymin = p.y();}
		if (first || p.y() > ymax) {ymax = p.y();}

		first = false;
	}
	return QRectF(QPointF(xmin, ymin), QPointF(xmax, ymax));
}
