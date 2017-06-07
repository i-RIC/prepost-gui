#include "leftbankhwm.h"
#include "leftbankhwmpreprocessorview.h"
#include "leftbankhwmpreprocessorviewhelper.h"
#include "../points/pointspreprocessorviewnameshelper.h"

#include <QColor>

LeftBankHWMPreProcessorView::LeftBankHWMPreProcessorView(Model* model, LeftBankHWM* item) :
	PointsPreProcessorView {model, item}
{
	viewHelpers().push_back(new LeftBankHWMPreProcessorViewHelper(this));
	viewHelpers().push_back(new PointsPreProcessorViewNamesHelper(this));
}

LeftBankHWMPreProcessorView::~LeftBankHWMPreProcessorView()
{}

void LeftBankHWMPreProcessorView::drawMarker(const QPointF &position, QPainter* painter)
{
	PointsPreProcessorViewHelper::drawRect(position, PointsPreProcessorViewHelper::STD_SIZE, Qt::blue, 0, painter);
}
