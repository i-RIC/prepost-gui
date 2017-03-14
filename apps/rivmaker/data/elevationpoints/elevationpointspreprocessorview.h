#ifndef ELEVATIONPOINTSPREPROCESSORVIEW_H
#define ELEVATIONPOINTSPREPROCESSORVIEW_H

#include "../base/dataitemview.h"

class ElevationPoints;

class ElevationPointsPreprocessorView : public DataItemView
{
public:
	ElevationPointsPreprocessorView(Model* model, ElevationPoints* item);
	~ElevationPointsPreprocessorView();

private:
	QRectF doBoundingBox() const override;
};

#endif // ELEVATIONPOINTSPREPROCESSORVIEW_H
