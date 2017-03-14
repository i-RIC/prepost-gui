#ifndef ELEVATIONPOINTSPREPROCESSORVIEW_H
#define ELEVATIONPOINTSPREPROCESSORVIEW_H

#include "elevationpoints.h"
#include "../base/dataitemview.h"

class ElevationPointsPreprocessorView : public DataItemView
{
public:
	ElevationPointsPreprocessorView(Model* model, ElevationPoints* item);
	~ElevationPointsPreprocessorView();

private:
	QRectF doBoundingBox() const override;
};

#endif // ELEVATIONPOINTSPREPROCESSORVIEW_H
