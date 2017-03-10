#ifndef WATERSURFACEELEVATIONPOINTS_H
#define WATERSURFACEELEVATIONPOINTS_H

#include <vector>

class QVector3D;

class WaterSurfaceElevationPoints
{
public:
	WaterSurfaceElevationPoints();
	~WaterSurfaceElevationPoints();

	const std::vector<QVector3D*>& leftBankPoints() const;
	std::vector<QVector3D*>& leftBankPoints();

	const std::vector<QVector3D*>& rightBankPoints() const;
	std::vector<QVector3D*>& rightBankPoints();

	const std::vector<QVector3D*>& arbitraryPoints() const;
	std::vector<QVector3D*> arbitraryPoints();

private:
	std::vector<QVector3D*> m_leftBankPoints;
	std::vector<QVector3D*> m_rightBankPoints;
	std::vector<QVector3D*> m_arbitraryPoints;
};

#endif // WATERSURFACEELEVATIONPOINTS_H
