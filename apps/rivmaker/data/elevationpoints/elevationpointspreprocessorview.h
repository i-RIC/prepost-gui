#ifndef ELEVATIONPOINTSPREPROCESSORVIEW_H
#define ELEVATIONPOINTSPREPROCESSORVIEW_H

#include "elevationpoints.h"
#include "../points/pointspreprocessorview.h"

class ElevationPointsPreprocessorView : public PointsPreProcessorView
{
public:
	ElevationPointsPreprocessorView(Model* model, ElevationPoints* item);
	~ElevationPointsPreprocessorView();

private:
	void drawMarker(const QPointF& position, QPainter* painter) const override;
};

#endif // ELEVATIONPOINTSPREPROCESSORVIEW_H
