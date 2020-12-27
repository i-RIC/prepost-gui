#ifndef GEODATAPOLYLINEGROUPPOLYLINE_H
#define GEODATAPOLYLINEGROUPPOLYLINE_H

#include "gd_polylinegroup_global.h"

#include <geodata/polydatagroup/geodatapolydatagrouppolydatawithboundingrect.h>

#include <vector>

namespace geos {
namespace geom {
class LineString;
} // geom
} // geos

class GeoDataPolyLineGroup;

class QDataStream;
class QPointF;
class QString;
class QVariant;

class GD_POLYLINEGROUP_EXPORT GeoDataPolyLineGroupPolyLine : public GeoDataPolyDataGroupPolyDataWithBoundingRect
{
	Q_OBJECT

public:
	GeoDataPolyLineGroupPolyLine(GeoDataPolyLineGroup* group);
	GeoDataPolyLineGroupPolyLine(const std::vector<QPointF>& line, GeoDataPolyLineGroup* group);
	GeoDataPolyLineGroupPolyLine(geos::geom::LineString* lineString, GeoDataPolyLineGroup* group);
	~GeoDataPolyLineGroupPolyLine();

	bool intersectWithLine(const QPointF& p1, const QPointF& p2, QPointF* intersection, double* r) const;

	geos::geom::LineString* getGeosLineString() const;
	void setGeosLineString(geos::geom::LineString* ls);

	std::vector<QPointF> points() const;
	std::vector<unsigned int> lineEdges() const;

	void copyShapeFrom(GeoDataPolyData* data) override;
	void copyShapeTo(GeoDataPolyData* data) override;

	void loadExternalData(QDataStream* stream);
	void saveExternalData(QDataStream* stream);

	void applyOffset(double x, double y);

private slots:
	void applyShape();

signals:
	void modified();

private:
	void setupBoundingRect();

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/geodatapolylinegrouppolyline_impl.h"
#endif

#endif // GEODATAPOLYLINEGROUPPOLYLINE_H
