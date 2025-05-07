#ifndef GEODATAGDALREAL_H
#define GEODATAGDALREAL_H

#include "geodatagdalt.h"

#include <vtkDoubleArray.h>

class GeoDataGdalReal : public GeoDataGdalT<double, vtkDoubleArray>
{
public:
	GeoDataGdalReal(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition);

	double missingValue() const override;
	double thresholdValue() const override;
};

#endif // GEODATAGDALREAL_H
