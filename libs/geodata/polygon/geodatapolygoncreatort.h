#ifndef GEODATAPOLYGONCREATORT_H
#define GEODATAPOLYGONCREATORT_H

#include "geodatapolygoncreator.h"

template <class V, class DA>
class GeoDataPolygonCreatorT : public GeoDataPolygonCreator
{
public:
	GeoDataPolygonCreatorT();
	virtual ~GeoDataPolygonCreatorT();

	bool isCompatibleWith(SolverDefinitionGridAttribute* condition) override;
};

#include "private/geodatapolygoncreatort_detail.h"

#endif // GEODATAPOLYGONCREATORT_H
