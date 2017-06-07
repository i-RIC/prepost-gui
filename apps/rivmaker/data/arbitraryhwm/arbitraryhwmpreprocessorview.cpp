#include "arbitraryhwm.h"
#include "arbitraryhwmpreprocessorview.h"
#include "arbitraryhwmpreprocessorviewhelper.h"
#include "../base/view.h"
#include "../points/pointspreprocessorviewnameshelper.h"

#include <QColor>
#include <QSize>

ArbitraryHWMPreProcessorView::ArbitraryHWMPreProcessorView(Model* model, ArbitraryHWM* item) :
	PointsPreProcessorView {model, item}
{
	viewHelpers().push_back(new ArbitraryHWMPreProcessorViewHelper(this));
	viewHelpers().push_back(new PointsPreProcessorViewNamesHelper(this));
}

ArbitraryHWMPreProcessorView::~ArbitraryHWMPreProcessorView()
{}

void ArbitraryHWMPreProcessorView::drawMarker(const QPointF &position, QPainter* painter) const
{
	PointsPreProcessorViewHelper::drawRect(position, PointsPreProcessorViewHelper::STD_SIZE, Qt::gray, 0, painter);
}

void ArbitraryHWMPreProcessorView::doDraw(QPainter *painter) const
{
	DataItemView::doDraw(painter);
	QSize size = view()->size();

	drawLegend(QPointF(size.width() - LEGEND_WIDTH, LEGEND_TOPMARGIN), painter);
}
