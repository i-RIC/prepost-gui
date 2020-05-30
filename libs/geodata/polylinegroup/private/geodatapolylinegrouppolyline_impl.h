#ifndef GEODATAPOLYLINEGROUPPOLYLINE_IMPL_H
#define GEODATAPOLYLINEGROUPPOLYLINE_IMPL_H

#include <QVariant>

#include <memory>
#include <vector>

namespace geos {
	namespace geom {
		class LineString;
	} // geom
} // geos

#include "../geodatapolylinegrouppolyline.h"

class GeoDataPolyLineGroupPolyLine::Impl
{
public:
	std::unique_ptr<geos::geom::LineString> m_polyLine;
};

#endif // GEODATAPOLYLINEGROUPPOLYLINE_IMPL_H
