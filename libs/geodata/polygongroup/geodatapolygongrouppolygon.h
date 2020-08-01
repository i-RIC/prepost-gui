#ifndef GEODATAPOLYGONGROUPPOLYGON_H
#define GEODATAPOLYGONGROUPPOLYGON_H

#include <geodata/polydatagroup/geodatapolydatagrouppolydatawithboundingrect.h>

#include <vector>

namespace geos {
namespace geom {
class Polygon;
} // geom
} // geos

class GeoDataPolygonGroup;

class QDataStream;
class QPolygonF;
class QPointF;
class QString;
class QVariant;

class GeoDataPolygonGroupPolygon : public GeoDataPolyDataGroupPolyDataWithBoundingRect
{
public:
	GeoDataPolygonGroupPolygon(GeoDataPolygonGroup* group);
	GeoDataPolygonGroupPolygon(const QPolygonF& outer, const std::vector<QPolygonF>& holes, GeoDataPolygonGroup* group);
	GeoDataPolygonGroupPolygon(geos::geom::Polygon* polygon, GeoDataPolygonGroup* group);
	~GeoDataPolygonGroupPolygon();

	bool isInside(const QPointF& point) const;
	geos::geom::Polygon* geosPolygon() const;
	void setGeosPolygon(geos::geom::Polygon* polygon);

	std::vector<QPointF> points() const;
	std::vector<unsigned int> lineEdges() const;
	std::vector<unsigned int> triangleCells() const;

	void copyShapeFrom(GeoDataPolyData* data) override;
	void copyShapeTo(GeoDataPolyData* data) override;

	void loadExternalData(QDataStream* stream) override;
	void saveExternalData(QDataStream* stream) override;

	void applyOffset(double x, double y) override;

private:
	void setupBoundingRect();
	void setupTriangleCells();

	class Impl;
	Impl* impl;

	class TriangulatorI;
	class TriangulatorGeos;
	class TriangulatorTriangle;
	class TriangulatorVtk;
};

#ifdef _DEBUG
	#include "private/geodatapolygongrouppolygon_impl.h"
#endif

#endif // GEODATAPOLYGONGROUPPOLYGON_H
