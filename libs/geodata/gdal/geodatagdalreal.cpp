#include "geodatanetcdfreal.h"

#include <vtkDoubleArray.h>

GeoDataNetcdfReal::GeoDataNetcdfReal(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition) :
	GeoDataNetcdfT<double, vtkDoubleArray> {d, creator, condition}
{}

double GeoDataNetcdfReal::missingValue() const
{
	return NC_FILL_DOUBLE;
}

double GeoDataNetcdfReal::thresholdValue() const
{
	double mval = missingValue();
	return mval * 0.999;
}
