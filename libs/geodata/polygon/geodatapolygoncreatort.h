#ifndef GEODATAPOLYGONCREATORT_H
#define GEODATAPOLYGONCREATORT_H

#include "geodatapolygoncreator.h"

template <class V, class DA>
class GeoDataPolygonCreatorT : public GeoDataPolygonCreator
{
public:
	GeoDataPolygonCreatorT(const QString& typeName);
	virtual ~GeoDataPolygonCreatorT();

	bool isCompatibleWith(SolverDefinitionGridAttribute* condition) const override;
};

#include "private/geodatapolygoncreatort_detail.h"

#endif // GEODATAPOLYGONCREATORT_H
