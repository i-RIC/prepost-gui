#ifndef ELEVATIONPOINTS_H
#define ELEVATIONPOINTS_H

#include "../points/points.h"

class ElevationPoints : public Points
{
	Q_OBJECT

public:
	ElevationPoints(DataItem* parent);
	~ElevationPoints();

	QStandardItem* buildPreProcessorStandardItem() const override;
	DataItemView* buildPreProcessorDataItemView(Model*) override;
};

#endif // ELEVATIONPOINTS_H
