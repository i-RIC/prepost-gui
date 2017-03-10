#ifndef ELEVATIONPOINTS_H
#define ELEVATIONPOINTS_H

#include <vector>

class QVector3D;

class ElevationPoints
{
public:
	ElevationPoints();
	~ElevationPoints();

	const std::vector<QVector3D*>& points() const;
	std::vector<QVector3D*>& points();

private:
	std::vector<QVector3D*> m_points;
};

#endif // ELEVATIONPOINTS_H
