#include "leftbankhwm.h"
#include "leftbankhwmpreprocessorview.h"
#include "leftbankhwmpreprocessorviewhelper.h"
#include "../base/view.h"
#include "../points/pointspreprocessorviewnameshelper.h"

#include <QColor>
#include <QSize>

LeftBankHWMPreProcessorView::LeftBankHWMPreProcessorView(Model* model, LeftBankHWM* item) :
	PointsPreProcessorView {model, item}
{
	viewHelpers().push_back(new LeftBankHWMPreProcessorViewHelper(this));
	viewHelpers().push_back(new PointsPreProcessorViewNamesHelper(this));
}

LeftBankHWMPreProcessorView::~LeftBankHWMPreProcessorView()
{}

void LeftBankHWMPreProcessorView::drawMarker(const QPointF &position, QPainter* painter) const
{
	PointsPreProcessorViewHelper::drawRect(position, PointsPreProcessorViewHelper::STD_SIZE, Qt::blue, 0, painter);
}

void LeftBankHWMPreProcessorView::doDraw(QPainter *painter) const
{
	DataItemView::doDraw(painter);
	QSize size = view()->size();

	drawLegend(QPointF(size.width() - LEGEND_WIDTH, LEGEND_TOPMARGIN + LEGEND_LINEHEIGHT * 4), painter);
}
