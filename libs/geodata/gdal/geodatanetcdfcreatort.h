#ifndef GEODATANETCDFCREATORT_H
#define GEODATANETCDFCREATORT_H

#include "geodatanetcdfcreator.h"
#include "geodatanetcdft.h"
#include "geodatanetcdfimportert.h"

class SolverDefinitionGridAttribute;

template <class V, class DA>
class GeoDataNetcdfCreatorT : public GeoDataNetcdfCreator
{
public:
	GeoDataNetcdfCreatorT(const QString& typeName);

	bool isCompatibleWith(SolverDefinitionGridAttribute* condition) const override;
};

#include "private/geodatanetcdfcreatort_detail.h"

#endif // GEODATANETCDFCREATORT_H
