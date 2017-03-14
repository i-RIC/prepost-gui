#include "crosssection.h"

CrossSection::CrossSection(DataItem* parent) :
	DataItem {parent}
{}

CrossSection::~CrossSection()
{}

QPointF CrossSection::point1() const
{
	return m_point1;
}

void CrossSection::setPoint1(const QPointF& p)
{
	m_point1 = p;
}

QPointF CrossSection::point2() const
{
	return m_point2;
}

void CrossSection::setPoint2(const QPointF& p)
{
	m_point2 = p;
}

void CrossSection::getNearestPoint(double x, double y, QPointF* nearestPoint, double* distance, double* pos) const
{

}

void CrossSection::reverseDirection()
{
	auto tmpP = m_point1;
	m_point1 = m_point2;
	m_point2 = tmpP;
}
