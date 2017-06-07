#include "rightbankhwm.h"
#include "rightbankhwmpreprocessorview.h"
#include "rightbankhwmpreprocessorviewhelper.h"
#include "../base/view.h"
#include "../points/pointspreprocessorviewnameshelper.h"

#include <QColor>
#include <QSize>

RightBankHWMPreProcessorView::RightBankHWMPreProcessorView(Model* model, RightBankHWM* item) :
	PointsPreProcessorView {model, item}
{
	viewHelpers().push_back(new RightBankHWMPreProcessorViewHelper(this));
	viewHelpers().push_back(new PointsPreProcessorViewNamesHelper(this));
}

RightBankHWMPreProcessorView::~RightBankHWMPreProcessorView()
{}

void RightBankHWMPreProcessorView::drawMarker(const QPointF &position, QPainter* painter) const
{
	PointsPreProcessorViewHelper::drawRect(position, PointsPreProcessorViewHelper::STD_SIZE, Qt::red, 0, painter);
}

void RightBankHWMPreProcessorView::doDraw(QPainter *painter) const
{
	DataItemView::doDraw(painter);
	QSize size = view()->size();

	drawLegend(QPointF(size.width() - LEGEND_WIDTH, LEGEND_TOPMARGIN + LEGEND_LINEHEIGHT * 5), painter);
}
