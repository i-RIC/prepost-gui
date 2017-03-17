#ifndef ELEVATIONPOINTSPREPROCESSORVIEWHELPER_H
#define ELEVATIONPOINTSPREPROCESSORVIEWHELPER_H

#include "../points/pointspreprocessorviewhelper.h"

class ElevationPointsPreProcessorViewHelper : public PointsPreProcessorViewHelper
{
public:
	ElevationPointsPreProcessorViewHelper(DataItemView* v);

	void draw(QPainter* painter) const override;
};

#endif // ELEVATIONPOINTSPREPROCESSORVIEWHELPER_H
