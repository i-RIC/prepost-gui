#include "hub.h"
#include "hubpreprocessorview.h"
#include "hubpreprocessorviewhelper.h"
#include "../base/view.h"
#include "../points/pointspreprocessorviewnameshelper.h"

#include <QColor>
#include <QSize>

HubPreProcessorView::HubPreProcessorView(Model* model, Hub* item) :
	PointsPreProcessorView {model, item}
{
	viewHelpers().push_back(new HubPreProcessorViewHelper(this));
	viewHelpers().push_back(new PointsPreProcessorViewNamesHelper(this));
}

HubPreProcessorView::~HubPreProcessorView()
{}

void HubPreProcessorView::drawMarker(const QPointF &position, QPainter* painter) const
{
	PointsPreProcessorViewHelper::drawDiamond(position, PointsPreProcessorViewHelper::STD_SIZE, Qt::gray, 0, painter);
}

void HubPreProcessorView::doDraw(QPainter *painter) const
{
	DataItemView::doDraw(painter);
	QSize size = view()->size();

	drawLegend(QPointF(size.width() - LEGEND_WIDTH, LEGEND_TOPMARGIN + LEGEND_LINEHEIGHT * 3), painter);
}
