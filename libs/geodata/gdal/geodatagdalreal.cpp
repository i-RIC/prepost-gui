#include "geodatagdalreal.h"

#include <vtkDoubleArray.h>

GeoDataGdalReal::GeoDataGdalReal(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition) :
	GeoDataGdalT<double, vtkDoubleArray> {d, creator, condition}
{}

double GeoDataGdalReal::missingValue() const
{
	return NC_FILL_DOUBLE;
}

double GeoDataGdalReal::thresholdValue() const
{
	double mval = missingValue();
	return mval * 0.999;
}
