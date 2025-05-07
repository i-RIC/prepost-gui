#ifndef GEODATAGDALINTEGER_H
#define GEODATAGDALINTEGER_H

#include "geodatagdalt.h"

#include <vtkIntArray.h>

class GeoDataGdalInteger : public GeoDataGdalT<int, vtkIntArray>
{
public:
	GeoDataGdalInteger(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition);

	int missingValue() const override;
	double thresholdValue() const override;
};

#endif // GEODATAGDALINTEGER_H
