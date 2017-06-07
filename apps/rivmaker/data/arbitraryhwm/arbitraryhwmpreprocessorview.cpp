#include "arbitraryhwm.h"
#include "arbitraryhwmpreprocessorview.h"
#include "arbitraryhwmpreprocessorviewhelper.h"
#include "../points/pointspreprocessorviewnameshelper.h"

#include <QColor>

ArbitraryHWMPreProcessorView::ArbitraryHWMPreProcessorView(Model* model, ArbitraryHWM* item) :
	PointsPreProcessorView {model, item}
{
	viewHelpers().push_back(new ArbitraryHWMPreProcessorViewHelper(this));
	viewHelpers().push_back(new PointsPreProcessorViewNamesHelper(this));
}

ArbitraryHWMPreProcessorView::~ArbitraryHWMPreProcessorView()
{}

void ArbitraryHWMPreProcessorView::drawMarker(const QPointF &position, QPainter* painter)
{
	PointsPreProcessorViewHelper::drawRect(position, PointsPreProcessorViewHelper::STD_SIZE, Qt::gray, 0, painter);
}
