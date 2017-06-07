#ifndef POINTSPREPROCESSORVIEW_H
#define POINTSPREPROCESSORVIEW_H

#include "points.h"
#include "../base/dataitemview.h"

class PointsPreProcessorView : public DataItemView
{
public:
	PointsPreProcessorView(Model* model, Points* item);
	virtual ~PointsPreProcessorView();

	void drawLegend(QPointF position, QPainter* painter);

private:
	virtual void drawMarker(const QPointF& position, QPainter* painter) = 0;
	QRectF doBoundingBox() const override;
};

#endif // POINTSPREPROCESSORVIEW_H
