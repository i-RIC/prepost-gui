#ifndef WATERSURFACEELEVATIONPOINTSPREPROCESSORVIEW_H
#define WATERSURFACEELEVATIONPOINTSPREPROCESSORVIEW_H

#include "watersurfaceelevationpoints.h"
#include "../base/dataitemview.h"

class WaterSurfaceElevationPointsPreProcessorView : public DataItemView
{
public:
	WaterSurfaceElevationPointsPreProcessorView(Model* model, WaterSurfaceElevationPoints* item);
	~WaterSurfaceElevationPointsPreProcessorView();

private:
	QRectF doBoundingBox() const override;
};

#endif // WATERSURFACEELEVATIONPOINTSPREPROCESSORVIEW_H
