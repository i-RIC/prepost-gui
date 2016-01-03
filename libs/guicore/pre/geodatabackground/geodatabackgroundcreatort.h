#ifndef GEODATABACKGROUNDCREATORT_H
#define GEODATABACKGROUNDCREATORT_H

#include "../geodata/geodatacreator.h"

template <class V, class DA>
class GeoDataBackgroundCreatorT : public GeoDataCreator
{
protected:
	GeoDataBackgroundCreatorT(const QString& typeName);

public:
	GeoData* create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition) override;
	bool isCompatibleWith(SolverDefinitionGridAttribute* condition) const override;
	QString defaultCaption(unsigned int index) const override;
};

#include "private/geodatabackgroundcreatort_detail.h"

#endif // GEODATABACKGROUNDCREATORT_H
