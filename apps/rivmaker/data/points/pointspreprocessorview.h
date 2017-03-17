#ifndef POINTSPREPROCESSORVIEW_H
#define POINTSPREPROCESSORVIEW_H

#include "points.h"
#include "../base/dataitemview.h"

class PointsPreProcessorView : public DataItemView
{
public:
	PointsPreProcessorView(Model* model, Points* item);
	virtual ~PointsPreProcessorView();

private:
	QRectF doBoundingBox() const override;
};

#endif // POINTSPREPROCESSORVIEW_H
