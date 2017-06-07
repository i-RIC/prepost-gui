#include "rightbankhwm.h"
#include "rightbankhwmpreprocessorview.h"
#include "rightbankhwmpreprocessorviewhelper.h"
#include "../points/pointspreprocessorviewnameshelper.h"

#include <QColor>

RightBankHWMPreProcessorView::RightBankHWMPreProcessorView(Model* model, RightBankHWM* item) :
	PointsPreProcessorView {model, item}
{
	viewHelpers().push_back(new RightBankHWMPreProcessorViewHelper(this));
	viewHelpers().push_back(new PointsPreProcessorViewNamesHelper(this));
}

RightBankHWMPreProcessorView::~RightBankHWMPreProcessorView()
{}

void RightBankHWMPreProcessorView::drawMarker(const QPointF &position, QPainter* painter)
{
	PointsPreProcessorViewHelper::drawRect(position, PointsPreProcessorViewHelper::STD_SIZE, Qt::red, 0, painter);
}
