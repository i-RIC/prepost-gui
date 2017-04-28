#include "crosssectionbuildermapalltonearest.h"

#include "../data/crosssection/crosssection.h"
#include "../data/crosssections/crosssections.h"
#include "../data/elevationpoints/elevationpoints.h"

#include <QVector2D>
#include <QVector3D>

#include <map>

CrossSectionBuilderMapAllToNearest::CrossSectionBuilderMapAllToNearest()
{}

void CrossSectionBuilderMapAllToNearest::build(const ElevationPoints& points, CrossSections* crossSections)
{
	for (QVector3D* p : points.points()) {
		std::multimap<double, CrossSection*> distanceMap;
		QPointF nearestPoint;
		double distance;
		for (CrossSection* cs : crossSections->crossSectionVector()) {
			cs->getNearestPoint(p->x(), p->y(), &nearestPoint, &distance);
			distanceMap.insert(std::make_pair(distance, cs));
		}
		auto it = distanceMap.begin();
		CrossSection* mappedCs = it->second;

		QPointF mappedPoint;
		double pos;
		mappedCs->getMappedPoint(p->x(), p->y(), &mappedPoint, &pos);
		mappedCs->addMappedPoint(QVector2D(pos, p->z()));
	}
}

