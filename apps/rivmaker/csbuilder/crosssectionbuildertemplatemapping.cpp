#include "crosssectionbuildertemplatemapping.h"

#include "../data/crosssection/crosssection.h"
#include "../data/crosssections/crosssections.h"
#include "../data/elevationpoints/elevationpoints.h"
#include "../geom/geometrypoint.h"

#include "private/crosssectionbuildertemplatemapping_impl.h"

#include <algorithm>

#include <QVector2D>

namespace {

const double VERYSMALL = 1.0E-12;

void rotate90(QVector2D& v)
{
	double tmp = v.x();
	v.setX(- v.y());
	v.setY(tmp);
}

bool mapElevation(const QPointF& p, const ElevationPoints& points, double streamWiseLength, double crossStreamWidth, int numberOfExpansions, double weightExponent, const QVector2D iDir, const QVector2D jDir, double* z)
{
	double searchRadius = 0;
	double searchIRadius = 0;
	double searchJRadius = 0;
	double radius = std::max(streamWiseLength, crossStreamWidth);

	for (int radInc = 0; radInc < numberOfExpansions; ++radInc) {
		searchRadius += radius;
		searchIRadius += streamWiseLength;
		searchJRadius += crossStreamWidth;

		double sum = 0;
		double rsum = 0;
		int pointsFound = 0;
		auto pointsInRect = points.pointsInRect(p.x() - searchRadius, p.y() - searchRadius, p.x() + searchRadius, p.y() + searchRadius);
		for (GeometryPoint* tmpp : pointsInRect) {
			QVector2D dv(tmpp->x() - p.x(), tmpp->y() - p.y());
			double deltaI = std::fabs(QVector2D::dotProduct(dv, iDir));
			double deltaJ = std::fabs(QVector2D::dotProduct(dv, jDir));
			if (deltaI < VERYSMALL && deltaJ < VERYSMALL) {
				*z = tmpp->z();
				return true;
			} else {
				double r = std::pow(dv.length(), weightExponent);
				sum += tmpp->z() / r;
				rsum += 1 / r;
				++ pointsFound;
			}
		}
		if (pointsFound > 0) {
			*z = sum / rsum;
			return true;
		}
	}

	return false;
}

void setupCrossSection(CrossSection* cs, const ElevationPoints& points, double resolution, double streamWiseLength, double crossStreamWidth, int numberOfExpansions, double weightExponent)
{
	cs->clearMappedPoints();

	QPointF p1 = cs->point1();
	QPointF p2 = cs->point2();

	QVector2D p1v(p1.x(), p1.y());
	QVector2D p2v(p2.x(), p2.y());

	double lenSqrt = (p2v - p1v).lengthSquared();
	QVector2D direction = (p2v - p1v).normalized();

	QVector2D jDir = direction;
	QVector2D iDir = jDir;
	rotate90(iDir);

	QVector2D deltaUnit = direction * resolution;

	QVector2D delta(0, 0);
	while (delta.lengthSquared() < lenSqrt) {
		QPointF p(p1.x() + delta.x(), p1.y() + delta.y());

		double z;
		bool mapped = mapElevation(p, points, streamWiseLength, crossStreamWidth, numberOfExpansions, weightExponent, iDir, jDir, &z);
		if (mapped) {
			double x = delta.length();
			cs->addMappedPoint(QVector2D(x, z));
		}
		delta = delta + deltaUnit;
	}
}

} // namespace

CrossSectionBuilderTemplateMapping::Impl::Impl() :
	m_mappingResolution {5},
	m_streamWiseLength {10},
	m_crossStreamWidth {5},
	m_numberOfExpansions {3},
	m_weightExponent {3}
{}

CrossSectionBuilderTemplateMapping::CrossSectionBuilderTemplateMapping() :
	impl {new Impl {}}
{}

CrossSectionBuilderTemplateMapping::~CrossSectionBuilderTemplateMapping()
{
	delete impl;
}

double CrossSectionBuilderTemplateMapping::mappingResolution() const
{
	return impl->m_mappingResolution;
}

void CrossSectionBuilderTemplateMapping::setMappingResolution(double resolution)
{
	impl->m_mappingResolution = resolution;
}

double CrossSectionBuilderTemplateMapping::streamWiseLength() const
{
	return impl->m_streamWiseLength;
}

void CrossSectionBuilderTemplateMapping::setStreamWiseLength(double len)
{
	impl->m_streamWiseLength = len;
}

double CrossSectionBuilderTemplateMapping::crossStreamWidth() const
{
	return impl->m_crossStreamWidth;
}

void CrossSectionBuilderTemplateMapping::setCrossStreamWidth(double width)
{
	impl->m_crossStreamWidth = width;
}

int CrossSectionBuilderTemplateMapping::numberOfExpansions() const
{
	return impl->m_numberOfExpansions;
}

void CrossSectionBuilderTemplateMapping::setNumberOfExpansions(int num)
{
	impl->m_numberOfExpansions = num;
}

double CrossSectionBuilderTemplateMapping::weightExponent() const
{
	return impl->m_weightExponent;
}

void CrossSectionBuilderTemplateMapping::setWeightExponent(double exp)
{
	impl->m_weightExponent = exp;
}

void CrossSectionBuilderTemplateMapping::build(const ElevationPoints& points, CrossSections* crossSections)
{
	for (CrossSection* cs : crossSections->crossSectionVector()) {
		setupCrossSection(cs, points, impl->m_mappingResolution, impl->m_streamWiseLength, impl->m_crossStreamWidth, impl->m_numberOfExpansions, impl->m_weightExponent);
	}
}
