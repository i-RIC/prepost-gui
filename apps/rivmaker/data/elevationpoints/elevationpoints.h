#ifndef ELEVATIONPOINTS_H
#define ELEVATIONPOINTS_H

#include "../points/points.h"

class ElevationPoints : public Points
{
	Q_OBJECT

public:
	ElevationPoints(DataItem* parent);
	~ElevationPoints();

	QString caption() const override;

	QStandardItem* buildPreProcessorStandardItem() const override;
	DataItemView* buildPreProcessorDataItemView(Model*) override;

	void setPoints(const std::vector<GeometryPoint*>& points) override;

	std::vector<GeometryPoint*> pointsInRect(double xmin, double ymin, double xmax, double ymax) const;
	std::vector<QVector2D> buildCrossSectionPoints(const QPointF& p1, const QPointF& p2) const;

private:
	void loadExternalData(const QString& filename) override;
	QString relativeFilename() const override;

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/elevationpoints_impl.h"
#endif // _DEBUG

#endif // ELEVATIONPOINTS_H
