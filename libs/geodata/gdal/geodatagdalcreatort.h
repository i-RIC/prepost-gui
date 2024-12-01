#ifndef GEODATAGDALCREATORT_H
#define GEODATAGDALCREATORT_H

#include "geodatagdalcreator.h"
#include "geodatagdalt.h"
#include "geodatagdalnetcdfimportert.h"

class SolverDefinitionGridAttribute;

template <class V, class DA>
class GeoDataGdalCreatorT : public GeoDataGdalCreator
{
public:
	GeoDataGdalCreatorT(const QString& typeName);

	bool isCompatibleWith(SolverDefinitionGridAttribute* condition) const override;
};

#include "private/geodatagdalcreatort_detail.h"

#endif // GEODATAGDALCREATORT_H
