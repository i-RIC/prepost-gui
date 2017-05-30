#ifndef CROSSSECTION_IMPL_H
#define CROSSSECTION_IMPL_H

#include "../crosssection.h"

#include <QPointF>
#include <QVector2D>

#include <map>

class CrossSection::Impl
{
public:
	Impl(CrossSection* cs);

	int m_id;
	bool m_isDefined;

	QPointF m_point1;
	QPointF m_point2;

	bool m_waterElevationIsSet;
	double m_waterElevation;

	std::map<double, double> m_mappedPoints;

private:
	CrossSection* m_parent;
};

#endif // CROSSSECTION_IMPL_H
