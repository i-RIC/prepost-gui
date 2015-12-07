#include "solverdefinitiongridattributereal.h"
#include "../pre/geodatabackground/geodatabackgroundrealcreator.h"

SolverDefinitionGridAttributeReal::SolverDefinitionGridAttributeReal(const QDomElement& elem, const SolverDefinitionTranslator& translator, Position pos, bool isOption, int order) :
	SolverDefinitionGridAttributeT<double> (elem, translator, pos, isOption, order)
{}

double SolverDefinitionGridAttributeReal::fromVariant(const QVariant& v) const
{
	return v.toDouble();
}

GeoData* SolverDefinitionGridAttributeReal::buildBackgroundGeoData(ProjectDataItem* parent)
{
	return GeoDataBackgroundRealCreator::instance()->create(parent, this);
}
