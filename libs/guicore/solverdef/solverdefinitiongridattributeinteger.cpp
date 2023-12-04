#include "solverdefinitiongridattributeinteger.h"
#include "../pre/geodatabackground/geodatabackgroundintegercreator.h"
#include "../pre/gridcond/container/gridattributeintegercontainer.h"
#include "../pre/gridcond/editwidget/gridattributeintegereditwidget.h"
#include "../pre/gridcond/editwidget/gridattributeintegervariationeditwidget.h"
#include "../pre/gridcond/stringconverter/gridattributestringconverterinteger.h"

SolverDefinitionGridAttributeInteger::SolverDefinitionGridAttributeInteger() :
	SolverDefinitionGridAttributeT<int> {}
{}

SolverDefinitionGridAttributeInteger::SolverDefinitionGridAttributeInteger(const QDomElement& elem, SolverDefinition* solverDef, Position pos, bool isOption, int order) :
	SolverDefinitionGridAttributeT<int> (elem, solverDef, pos, isOption, order)
{}

GridAttributeEditWidget* SolverDefinitionGridAttributeInteger::editWidget(QWidget* parent)
{
	return new GridAttributeIntegerEditWidget(parent, this);
}

GridAttributeVariationEditWidget* SolverDefinitionGridAttributeInteger::variationEditWidget(QWidget* parent)
{
	return new GridAttributeIntegerVariationEditWidget(parent, this);
}

GridAttributeContainer* SolverDefinitionGridAttributeInteger::buildContainer(v4InputGrid* grid)
{
	return new GridAttributeIntegerContainer(grid, this);
}

GridAttributeStringConverter* SolverDefinitionGridAttributeInteger::stringConverter() const
{
	return new GridAttributeStringConverterInteger();
}

int SolverDefinitionGridAttributeInteger::fromVariant(const QVariant& v) const
{
	return v.toInt();
}

GeoData* SolverDefinitionGridAttributeInteger::buildBackgroundGeoData(ProjectDataItem* parent)
{
	return GeoDataBackgroundIntegerCreator::instance()->create(parent, this);
}
