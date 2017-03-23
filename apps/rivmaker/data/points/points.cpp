#include "points.h"
#include "../../io/points/pointsimporter.h"
#include "../project/project.h"

#include <QVector3D>

Points::Points(DataItem* parent) :
	DataItem {parent}
{}

Points::~Points()
{
	clearPoints();
}

const std::vector<QVector3D*>& Points::points() const
{
	return m_points;
}

std::vector<QVector3D*>& Points::points()
{
	return m_points;
}

void Points::setPoints(const std::vector<QVector3D*>& points)
{
	clearPoints();
	m_points = points;
}

void Points::clearPoints()
{
	for (auto p : m_points) {
		delete p;
	}
	m_points.clear();
}

void Points::importData(QWidget* w)
{
	std::vector<QVector3D*> newPoints;
	QPointF offset = project()->offset();
	bool ok = PointsImporter::importData(&newPoints, &offset, w);
	if (! ok) {return;}

	clearPoints();
	m_points = newPoints;
	project()->setOffset(offset);
}
