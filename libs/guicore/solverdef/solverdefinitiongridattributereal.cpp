#include "solverdefinitiongridattributereal.h"
#include "../pre/geodatabackground/geodatabackgroundrealcreator.h"
#include "../pre/gridcond/stringconverter/gridattributestringconverterreal.h"

SolverDefinitionGridAttributeReal::SolverDefinitionGridAttributeReal() :
	SolverDefinitionGridAttributeT<double> {}
{}

SolverDefinitionGridAttributeReal::SolverDefinitionGridAttributeReal(const QDomElement& elem, SolverDefinition* solverDef, Position pos, bool isOption, int order) :
	SolverDefinitionGridAttributeT<double> (elem, solverDef, pos, isOption, order)
{}

GridAttributeStringConverter* SolverDefinitionGridAttributeReal::stringConverter() const
{
	return new GridAttributeStringConverterReal();
}

double SolverDefinitionGridAttributeReal::fromVariant(const QVariant& v) const
{
	return v.toDouble();
}

GeoData* SolverDefinitionGridAttributeReal::buildBackgroundGeoData(ProjectDataItem* parent)
{
	return GeoDataBackgroundRealCreator::instance()->create(parent, this);
}
