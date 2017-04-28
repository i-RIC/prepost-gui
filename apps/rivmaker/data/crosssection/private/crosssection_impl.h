#ifndef CROSSSECTION_IMPL_H
#define CROSSSECTION_IMPL_H

#include "../crosssection.h"

#include <QPointF>
#include <QVector2D>

class CrossSection::Impl
{
public:
	Impl(CrossSection* cs);

	int m_id;
	bool m_isDefined;

	QPointF m_point1;
	QPointF m_point2;

	double m_waterElevation;

	std::vector<QVector3D*> m_mappedPoints;

private:
	CrossSection* m_parent;
};

#endif // CROSSSECTION_IMPL_H
