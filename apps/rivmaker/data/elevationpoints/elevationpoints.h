#ifndef ELEVATIONPOINTS_H
#define ELEVATIONPOINTS_H

#include "../base/dataitem.h"

#include <vector>

class QVector3D;

class ElevationPoints : public DataItem
{
public:
	ElevationPoints(DataItem* parent);
	~ElevationPoints();

	const std::vector<QVector3D*>& points() const;
	std::vector<QVector3D*>& points();

private:
	std::vector<QVector3D*> m_points;
};

#endif // ELEVATIONPOINTS_H
