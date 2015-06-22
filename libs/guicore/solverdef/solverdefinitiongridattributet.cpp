#include "../pre/geodatabackground/geodatabackgroundintegercreator.h"
#include "../pre/geodatabackground/geodatabackgroundrealcreator.h"
#include "solverdefinitiongridattributet.h"

GeoData* SolverDefinitionGridAttributeInteger::buildBackgroundGeoData(ProjectDataItem* parent)
{
	return GeoDataBackgroundIntegerCreator::instance()->create(parent, this);
}

GeoData* SolverDefinitionGridAttributeReal::buildBackgroundGeoData(ProjectDataItem* parent)
{
	return GeoDataBackgroundRealCreator::instance()->create(parent, this);
}
