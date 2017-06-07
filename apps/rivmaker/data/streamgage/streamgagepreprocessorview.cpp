#include "streamgage.h"
#include "streamgagepreprocessorview.h"
#include "streamgagepreprocessorviewhelper.h"
#include "../base/view.h"
#include "../points/pointspreprocessorviewnameshelper.h"

#include <QColor>
#include <QSize>

StreamGagePreProcessorView::StreamGagePreProcessorView(Model* model, StreamGage* item) :
	PointsPreProcessorView {model, item}
{
	viewHelpers().push_back(new StreamGagePreProcessorViewHelper(this));
	viewHelpers().push_back(new PointsPreProcessorViewNamesHelper(this));
}

StreamGagePreProcessorView::~StreamGagePreProcessorView()
{}

void StreamGagePreProcessorView::drawMarker(const QPointF &position, QPainter* painter) const
{
	PointsPreProcessorViewHelper::drawRect(position, PointsPreProcessorViewHelper::STD_SIZE, Qt::gray, 0, painter);
}

void StreamGagePreProcessorView::doDraw(QPainter *painter) const
{
	DataItemView::doDraw(painter);
	QSize size = view()->size();

	drawLegend(QPointF(size.width() - LEGEND_WIDTH, LEGEND_TOPMARGIN + LEGEND_LINEHEIGHT * 6), painter);
}
