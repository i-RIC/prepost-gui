#include "hub.h"
#include "hubpreprocessorview.h"
#include "hubpreprocessorviewhelper.h"
#include "../points/pointspreprocessorviewnameshelper.h"

#include <QColor>

HubPreProcessorView::HubPreProcessorView(Model* model, Hub* item) :
	PointsPreProcessorView {model, item}
{
	viewHelpers().push_back(new HubPreProcessorViewHelper(this));
	viewHelpers().push_back(new PointsPreProcessorViewNamesHelper(this));
}

HubPreProcessorView::~HubPreProcessorView()
{}

void HubPreProcessorView::drawMarker(const QPointF &position, QPainter* painter)
{
	PointsPreProcessorViewHelper::drawDiamond(position, PointsPreProcessorViewHelper::STD_SIZE, Qt::gray, 0, painter);
}
