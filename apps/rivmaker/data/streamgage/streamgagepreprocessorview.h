#ifndef STREAMGAGEPREPROCESSORVIEW_H
#define STREAMGAGEPREPROCESSORVIEW_H

#include "../points/pointspreprocessorview.h"

class StreamGage;

class StreamGagePreProcessorView : public PointsPreProcessorView
{
public:
	StreamGagePreProcessorView(Model* model, StreamGage* item);
	~StreamGagePreProcessorView();

private:
	void drawMarker(const QPointF& position, QPainter* painter) const override;
	void doDraw(QPainter *painter) const override;
};

#endif // STREAMGAGEPREPROCESSORVIEW_H
