#include "geodatagdalinteger.h"

GeoDataGdalInteger::GeoDataGdalInteger(ProjectDataItem* d, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition) :
	GeoDataGdalT<int, vtkIntArray> {d, creator, condition}
{}

int GeoDataGdalInteger::missingValue() const
{
	return NC_FILL_INT;
}

double GeoDataGdalInteger::thresholdValue() const
{
	int mval = missingValue();
	if (mval > 0) {return mval - 1;}
	return mval + 1;
}
