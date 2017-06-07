#ifndef POINTSPREPROCESSORVIEW_H
#define POINTSPREPROCESSORVIEW_H

#include "points.h"
#include "../base/dataitemview.h"

class PointsPreProcessorView : public DataItemView
{
public:
	const static int LEGEND_WIDTH;
	const static int LEGEND_TOPMARGIN;
	const static int LEGEND_LINEHEIGHT;

	PointsPreProcessorView(Model* model, Points* item);
	virtual ~PointsPreProcessorView();

	void drawLegend(QPointF position, QPainter* painter) const;

private:
	virtual void drawMarker(const QPointF& position, QPainter* painter) const = 0;
	QRectF doBoundingBox() const override;
};

#endif // POINTSPREPROCESSORVIEW_H
