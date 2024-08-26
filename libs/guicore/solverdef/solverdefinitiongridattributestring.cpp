#include "../pre/geodatabackground/geodatabackgroundstringcreator.h"
#include "../pre/gridcond/container/gridattributestringcontainer.h"
#include "../pre/gridcond/editwidget/gridattributestringeditwidget.h"
#include "../pre/gridcond/stringconverter/gridattributestringconverterstring.h"
#include "solverdefinitiongridattributestring.h"

SolverDefinitionGridAttributeString::SolverDefinitionGridAttributeString() :
	SolverDefinitionGridAttributeT<std::string> {}
{}

SolverDefinitionGridAttributeString::SolverDefinitionGridAttributeString(const QDomElement& elem, SolverDefinition* solverDef, Position pos, bool isOption, int order) :
	SolverDefinitionGridAttributeT<std::string> (elem, solverDef, pos, isOption, order)
{}

GridAttributeEditWidget* SolverDefinitionGridAttributeString::editWidget(QWidget* parent)
{
	return new GridAttributeStringEditWidget(parent, this);
}

GridAttributeVariationEditWidget* SolverDefinitionGridAttributeString::variationEditWidget(QWidget* parent)
{
	return nullptr;
}

GridAttributeContainer* SolverDefinitionGridAttributeString::buildContainer(v4InputGrid* grid)
{
	return new GridAttributeStringContainer(grid, this);
}

GridAttributeStringConverter* SolverDefinitionGridAttributeString::stringConverter() const
{
	return new GridAttributeStringConverterString();
}

std::string SolverDefinitionGridAttributeString::fromVariant(const QVariant& v) const
{
	return iRIC::toStr(v.toString());
}

GeoData* SolverDefinitionGridAttributeString::buildBackgroundGeoData(ProjectDataItem* parent)
{
	return GeoDataBackgroundStringCreator::instance()->create(parent, this);
}
