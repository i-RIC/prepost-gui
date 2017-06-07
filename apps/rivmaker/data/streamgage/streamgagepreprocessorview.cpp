#include "streamgage.h"
#include "streamgagepreprocessorview.h"
#include "streamgagepreprocessorviewhelper.h"
#include "../points/pointspreprocessorviewnameshelper.h"

#include <QColor>

StreamGagePreProcessorView::StreamGagePreProcessorView(Model* model, StreamGage* item) :
	PointsPreProcessorView {model, item}
{
	viewHelpers().push_back(new StreamGagePreProcessorViewHelper(this));
	viewHelpers().push_back(new PointsPreProcessorViewNamesHelper(this));
}

StreamGagePreProcessorView::~StreamGagePreProcessorView()
{}

void StreamGagePreProcessorView::drawMarker(const QPointF &position, QPainter* painter)
{
	PointsPreProcessorViewHelper::drawRect(position, PointsPreProcessorViewHelper::STD_SIZE, Qt::gray, 0, painter);
}
