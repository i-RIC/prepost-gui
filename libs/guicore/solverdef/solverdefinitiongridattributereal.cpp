#include "solverdefinitiongridattributereal.h"
#include "../pre/geodatabackground/geodatabackgroundrealcreator.h"

SolverDefinitionGridAttributeReal::SolverDefinitionGridAttributeReal(const QDomElement& elem, SolverDefinition* solverDef, Position pos, bool isOption, int order) :
	SolverDefinitionGridAttributeT<double> (elem, solverDef, pos, isOption, order)
{}

double SolverDefinitionGridAttributeReal::fromVariant(const QVariant& v) const
{
	return v.toDouble();
}

GeoData* SolverDefinitionGridAttributeReal::buildBackgroundGeoData(ProjectDataItem* parent)
{
	return GeoDataBackgroundRealCreator::instance()->create(parent, this);
}
