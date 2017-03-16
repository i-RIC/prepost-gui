#include "crosssection.h"
#include "crosssectionpreprocessorview.h"
#include "crosssectionpreprocessorviewhelper.h"

#include <QRectF>

CrossSectionPreProcessorView::CrossSectionPreProcessorView(Model* model, CrossSection* item) :
	DataItemView {model, item}
{
	viewHelpers().push_back(new CrossSectionPreProcessorViewHelper(this));
}

CrossSectionPreProcessorView::~CrossSectionPreProcessorView()
{}

QRectF CrossSectionPreProcessorView::doBoundingBox() const
{
	double xmin, xmax, ymin, ymax;

	auto cs = dynamic_cast<CrossSection*> (item());
	xmin = xmax = cs->point1().x();
	ymin = ymax = cs->point1().y();

	if (cs->point2().x() < xmin) {xmin = cs->point2().x();}
	if (cs->point2().x() > xmax) {xmax = cs->point2().x();}
	if (cs->point2().y() < ymin) {ymin = cs->point2().y();}
	if (cs->point2().y() > ymax) {ymax = cs->point2().y();}

	return QRectF(QPointF(xmin, ymin), QPointF(xmax, ymax));
}
