#ifndef ARBITRARYHWMPREPROCESSORVIEW_H
#define ARBITRARYHWMPREPROCESSORVIEW_H

#include "../points/pointspreprocessorview.h"

class ArbitraryHWM;

class ArbitraryHWMPreProcessorView : public PointsPreProcessorView
{
public:
	ArbitraryHWMPreProcessorView(Model* model, ArbitraryHWM* item);
	~ArbitraryHWMPreProcessorView();

private:
	void drawMarker(const QPointF& position, QPainter* painter) const override;
	void doDraw(QPainter *painter) const override;
};

#endif // ARBITRARYHWMPREPROCESSORVIEW_H
