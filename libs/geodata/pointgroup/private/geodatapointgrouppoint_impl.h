#ifndef GEODATAPOINTGROUPPOINT_IMPL_H
#define GEODATAPOINTGROUPPOINT_IMPL_H

#include <QPointF>
#include <QVariant>

#include <memory>
#include <vector>

namespace geos {
	namespace geom {
		class LineString;
	} // geom
} // geos

#include "../geodatapointgrouppoint.h"

class GeoDataPointGroupPoint::Impl
{
public:
	QPointF m_point;
};

#endif // GEODATAPOINTGROUPPOINT_IMPL_H
