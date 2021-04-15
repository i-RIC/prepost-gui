#ifndef GEODATANETCDFINTEGER_H
#define GEODATANETCDFINTEGER_H

#include "geodatanetcdft.h"

#include <vtkIntArray.h>

class GeoDataNetcdfInteger : public GeoDataNetcdfT<int, vtkIntArray>
{
public:
	GeoDataNetcdfInteger(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition);

	int missingValue() const override;
	double thresholdValue() const override;
};

#endif // GEODATANETCDFINTEGER_H
