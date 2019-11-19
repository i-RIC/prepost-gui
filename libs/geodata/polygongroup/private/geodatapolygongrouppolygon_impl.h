#ifndef GEODATAPOLYGONGROUPPOLYGON_IMPL_H
#define GEODATAPOLYGONGROUPPOLYGON_IMPL_H

#include <QVariant>

#include <memory>
#include <vector>

namespace geos {
	namespace geom {
		class Polygon;
	} // geom
} // geos

#include "../geodatapolygongrouppolygon.h"

class GeoDataPolygonGroupPolygon::Impl
{
public:
	std::unique_ptr<geos::geom::Polygon> m_polygon;
	std::vector<unsigned int> m_triangleCells;

	QString m_name;
	QVariant m_value;

	double m_xmin;
	double m_xmax;
	double m_ymin;
	double m_ymax;

	unsigned int m_indexOffset;
};

#endif // GEODATAPOLYGONGROUPPOLYGON_IMPL_H
