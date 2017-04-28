#include "crosssectionbuildertin.h"

#include "../data/crosssection/crosssection.h"
#include "../data/crosssections/crosssections.h"
#include "../data/elevationpoints/elevationpoints.h"

#include <QPointF>
#include <QVector2D>

CrossSectionBuilderTin::CrossSectionBuilderTin()
{}

void CrossSectionBuilderTin::build(const ElevationPoints& points, CrossSections* crossSections)
{
	for (CrossSection* cs : crossSections->crossSectionVector()) {
		std::vector<QVector2D> csPoints = points.buildCrossSectionPoints(cs->point1(), cs->point2());
		cs->setMappedPoints(csPoints);
	}
}
