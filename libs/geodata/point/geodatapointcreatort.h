#ifndef GEODATAPOINTCREATORT_H
#define GEODATAPOINTCREATORT_H

#include "geodatapointcreator.h"

template <class V, class DA>
class GeoDataPointCreatorT : public GeoDataPointCreator
{
public:
	GeoDataPointCreatorT(const QString& typeName);
	~GeoDataPointCreatorT() override;

	GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;
	bool isCompatibleWith(SolverDefinitionGridAttribute* condition) const override;
};

#include "private/geodatapointcreatort_detail.h"

#endif // GEODATAPOINTCREATORT_H
