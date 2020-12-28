#ifndef GEODATAPOINTGROUPPOINT_H
#define GEODATAPOINTGROUPPOINT_H

#include "gd_pointgroup_global.h"

#include <geodata/polydatagroup/geodatapolydatagrouppolydata.h>

#include <vector>

namespace geos {
namespace geom {
class LineString;
} // geom
} // geos

class GeoDataPointGroup;

class QDataStream;
class QPointF;
class QString;
class QVariant;

class GeoDataPointGroupPoint : public GeoDataPolyDataGroupPolyData
{
	Q_OBJECT

public:
	GeoDataPointGroupPoint(GeoDataPointGroup* group);
	GeoDataPointGroupPoint(const QPointF& point, GeoDataPointGroup* group);
	~GeoDataPointGroupPoint();

	QPointF point() const;
	void setPoint(const QPointF& point);

	void getBoundingRect(double* xmin, double* xmax, double* ymin, double* ymax) override;
	void copyShapeFrom(GeoDataPolyData* data) override;
	void copyShapeTo(GeoDataPolyData* data) override;

	void loadExternalData(QDataStream* stream) override;
	void saveExternalData(QDataStream* stream) override;

	void applyOffset(double x, double y) override;

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/geodatapointgrouppoint_impl.h"
#endif

#endif // GEODATAPOINTGROUPPOINT_H
