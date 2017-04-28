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

	void setPoints(const std::vector<QVector3D*>& points) override;

	std::vector<QVector2D> buildCrossSectionPoints(const QPointF& p1, const QPointF& p2) const;

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/elevationpoints_impl.h"
#endif // _DEBUG

#endif // ELEVATIONPOINTS_H
