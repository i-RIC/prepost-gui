#include "geodatanetcdfinteger.h"

GeoDataNetcdfInteger::GeoDataNetcdfInteger(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition) :
	GeoDataNetcdfT<int, vtkIntArray> {d, creator, condition}
{}

int GeoDataNetcdfInteger::missingValue() const
{
	return NC_FILL_INT;
}

double GeoDataNetcdfInteger::thresholdValue() const
{
	int mval = missingValue();
	if (mval > 0) {return mval - 1;}
	return mval + 1;
}
