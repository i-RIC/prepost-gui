#ifndef GEODATAPOLYGONGROUPREADONLYCREATORT_H
#define GEODATAPOLYGONGROUPREADONLYCREATORT_H

#include "geodatapolygongroupreadonlycreator.h"

template <typename V, typename DA>
class GeoDataPolygonGroupReadOnlyCreatorT : public GeoDataPolygonGroupReadOnlyCreator
{
public:
	GeoDataPolygonGroupReadOnlyCreatorT(const QString& typeName);
	virtual ~GeoDataPolygonGroupReadOnlyCreatorT();

	GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;
	bool isCompatibleWith(SolverDefinitionGridAttribute *condition) const override;
};

#include "private/geodatapolygongroupcreadonlycreatort_detail.h"

#endif // GEODATAPOLYGONGROUPREADONLYCREATORT_H
