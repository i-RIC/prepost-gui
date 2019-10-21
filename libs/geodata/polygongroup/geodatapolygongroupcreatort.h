#ifndef GEODATAPOLYGONGROUPCREATORT_H
#define GEODATAPOLYGONGROUPCREATORT_H

#include "geodatapolygongroupcreator.h"

template <typename V, typename DA>
class GeoDataPolygonGroupCreatorT : public GeoDataPolygonGroupCreator
{
public:
	GeoDataPolygonGroupCreatorT(const QString& typeName);
	virtual ~GeoDataPolygonGroupCreatorT();

	bool isCompatibleWith(SolverDefinitionGridAttribute *condition) const override;
};

#include "private/geodatapolygongroupcreatort_detail.h"

#endif // GEODATAPOLYGONGROUPCREATORT_H
