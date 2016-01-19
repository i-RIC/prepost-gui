#ifndef GEODATANETCDFREAL_H
#define GEODATANETCDFREAL_H

#include "geodatanetcdft.h"

#include <vtkDoubleArray.h>

class GeoDataNetcdfReal : public GeoDataNetcdfT<double, vtkDoubleArray>
{
public:
	GeoDataNetcdfReal(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition);

	double missingValue() const override;
	double thresholdValue() const override;
};

#endif // GEODATANETCDFREAL_H
