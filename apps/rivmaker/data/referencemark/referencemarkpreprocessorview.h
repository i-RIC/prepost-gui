#ifndef REFERENCEMARKPREPROCESSORVIEW_H
#define REFERENCEMARKPREPROCESSORVIEW_H

#include "../points/pointspreprocessorview.h"

class ReferenceMark;

class ReferenceMarkPreProcessorView : public PointsPreProcessorView
{
public:
	ReferenceMarkPreProcessorView(Model* model, ReferenceMark* item);
	~ReferenceMarkPreProcessorView();

private:
	void drawMarker(const QPointF& position, QPainter* painter) const override;
	void doDraw(QPainter *painter) const override;
};

#endif // REFERENCEMARKPREPROCESSORVIEW_H
