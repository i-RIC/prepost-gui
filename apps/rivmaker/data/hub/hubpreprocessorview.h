#ifndef HUBPREPROCESSORVIEW_H
#define HUBPREPROCESSORVIEW_H

#include "../points/pointspreprocessorview.h"

class Hub;

class HubPreProcessorView : public PointsPreProcessorView
{
public:
	HubPreProcessorView(Model* model, Hub* item);
	~HubPreProcessorView();

private:
	void drawMarker(const QPointF& position, QPainter* painter) const override;
	void doDraw(QPainter *painter) const override;
};

#endif // HUBPREPROCESSORVIEW_H
