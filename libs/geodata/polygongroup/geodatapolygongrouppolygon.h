#ifndef GEODATAPOLYGONGROUPPOLYGON_H
#define GEODATAPOLYGONGROUPPOLYGON_H

#include <vector>

namespace geos {
namespace geom {
class Polygon;
} // geom
} // geos

class QDataStream;
class QPolygonF;
class QPointF;
class QString;
class QVariant;

class GeoDataPolygonGroupPolygon
{
public:
	GeoDataPolygonGroupPolygon();
	GeoDataPolygonGroupPolygon(const QPolygonF& outer, const std::vector<QPolygonF>& holes);
	GeoDataPolygonGroupPolygon(geos::geom::Polygon* polygon);
	~GeoDataPolygonGroupPolygon();

	QString name() const;
	void setName(const QString& name);

	QVariant value() const;
	void setValue(const QVariant& v);

	bool isInside(const QPointF& point) const;
	geos::geom::Polygon* geosPolygon() const;

	std::vector<QPointF> points() const;
	std::vector<unsigned int> lineEdges() const;
	std::vector<unsigned int> triangleCells() const;

	unsigned int indexOffset() const;
	void setIndexOffset(unsigned int offset);

	void getBoundingRect(double* xmin, double* xmax, double* ymin, double* ymax);

	void loadExternalData(QDataStream* stream);
	void saveExternalData(QDataStream* stream);

	void applyOffset(double x, double y);

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
