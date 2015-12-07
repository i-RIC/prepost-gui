#include "solverdefinitiongridattributeinteger.h"
#include "../pre/geodatabackground/geodatabackgroundintegercreator.h"

SolverDefinitionGridAttributeInteger::SolverDefinitionGridAttributeInteger(const QDomElement& elem, const SolverDefinitionTranslator& translator, Position pos, bool isOption, int order) :
	SolverDefinitionGridAttributeT<int> (elem, translator, pos, isOption, order)
{}

int SolverDefinitionGridAttributeInteger::fromVariant(const QVariant& v) const
{
	return v.toInt();
}

GeoData* SolverDefinitionGridAttributeInteger::buildBackgroundGeoData(ProjectDataItem* parent)
{
	return GeoDataBackgroundIntegerCreator::instance()->create(parent, this);
}
