#ifndef ELEVATIONPOINTSPREPROCESSORVIEWHELPER_H
#define ELEVATIONPOINTSPREPROCESSORVIEWHELPER_H

#include "../base/dataitemviewhelperi.h"

class ElevationPointsPreProcessorViewHelper : public DataItemViewHelperI
{
public:
	ElevationPointsPreProcessorViewHelper(DataItemView* v);

	void draw(QPainter* painter) const override;
};

#endif // ELEVATIONPOINTSPREPROCESSORVIEWHELPER_H
