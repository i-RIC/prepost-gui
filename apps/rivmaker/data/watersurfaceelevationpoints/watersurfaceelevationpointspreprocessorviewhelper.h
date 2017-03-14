#ifndef WATERSURFACEELEVATIONPOINTSPREPROCESSORVIEWHELPER_H
#define WATERSURFACEELEVATIONPOINTSPREPROCESSORVIEWHELPER_H

#include "../base/dataitemviewhelperi.h"

class WaterSurfaceElevationPointsPreProcessorViewHelper : public DataItemViewHelperI
{
public:
	WaterSurfaceElevationPointsPreProcessorViewHelper(DataItemView* v);

	void draw(QPainter* painter) const override;
};

#endif // WATERSURFACEELEVATIONPOINTSPREPROCESSORVIEWHELPER_H
