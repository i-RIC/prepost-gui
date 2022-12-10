#include "coordinatesystem.h"
#include "coordinatesystemconverter.h"
#include "private/coordinatesystemconverter_impl.h"

#include <misc/stringtool.h>

#include <proj_api.h>

CoordinateSystemConverter::CoordinateSystemConverter(CoordinateSystem* src, CoordinateSystem* tgt) :
	impl {new Impl {}}
{
	impl->m_sourceCs = src;
	impl->m_targetCs = tgt;

	impl->m_sourcePj = pj_init_plus(iRIC::toStr(src->proj4PlaneStr()).c_str());
	impl->m_targetPj = pj_init_plus(iRIC::toStr(tgt->proj4PlaneStr()).c_str());
}

CoordinateSystemConverter::~CoordinateSystemConverter()
{
	delete impl;
}

QPointF CoordinateSystemConverter::convert(const QPointF& point)
{
	double x = point.x();
	double y = point.y();

	if (impl->m_sourceCs->isLongLat()) {
		x *= DEG_TO_RAD;
		y *= DEG_TO_RAD;
	}

	int result = pj_transform(impl->m_sourcePj, impl->m_targetPj, 1, 1, &x, &y, nullptr);
	// char* error_message = pj_strerrno(result);

	if (impl->m_targetCs->isLongLat()) {
		x *= RAD_TO_DEG;
		y *= RAD_TO_DEG;
	}

	return QPointF(x, y);
}

std::vector<QPointF> CoordinateSystemConverter::convert(const std::vector<QPointF>& points)
{
	std::vector<double> x;
	std::vector<double> y;

	x.assign(points.size(), 0);
	y.assign(points.size(), 0);

	for (int i = 0; i < points.size(); ++i) {
		const auto& p = points[i];
		x[i] = p.x();
		y[i] = p.y();

		if (impl->m_sourceCs->isLongLat()) {
			x[i] *= DEG_TO_RAD;
			y[i] *= DEG_TO_RAD;
		}
	}
	int result = pj_transform(impl->m_sourcePj, impl->m_targetPj, points.size(), 1, x.data(), y.data(), nullptr);

	std::vector<QPointF> ret;
	ret.assign(points.size(), QPointF(0, 0));
	for (int i = 0; i < points.size(); ++i) {
		if (impl->m_targetCs->isLongLat()) {
			x[i] *= RAD_TO_DEG;
			y[i] *= RAD_TO_DEG;
		}

		QPointF p2(x[i], y[i]);
		ret[i] = p2;
	}
	return ret;
}
