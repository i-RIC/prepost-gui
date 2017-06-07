#include "referencemark.h"
#include "referencemarkpreprocessorview.h"
#include "referencemarkpreprocessorviewhelper.h"
#include "../points/pointspreprocessorviewnameshelper.h"

#include <QColor>

ReferenceMarkPreProcessorView::ReferenceMarkPreProcessorView(Model* model, ReferenceMark* item) :
	PointsPreProcessorView {model, item}
{
	viewHelpers().push_back(new ReferenceMarkPreProcessorViewHelper(this));
	viewHelpers().push_back(new PointsPreProcessorViewNamesHelper(this));
}

ReferenceMarkPreProcessorView::~ReferenceMarkPreProcessorView()
{}

void ReferenceMarkPreProcessorView::drawMarker(const QPointF &position, QPainter* painter)
{
	PointsPreProcessorViewHelper::drawTriangle(position, PointsPreProcessorViewHelper::STD_SIZE, Qt::black, 0, painter);
}
