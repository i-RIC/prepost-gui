#include "solverdefinitiongridattributereal.h"
#include "../pre/geodatabackground/geodatabackgroundrealcreator.h"
#include "../pre/gridcond/container/gridattributerealcontainer.h"
#include "../pre/gridcond/editwidget/gridattributerealeditwidget.h"
#include "../pre/gridcond/editwidget/gridattributerealvariationeditwidget.h"
#include "../pre/gridcond/stringconverter/gridattributestringconverterreal.h"

SolverDefinitionGridAttributeReal::SolverDefinitionGridAttributeReal() :
	SolverDefinitionGridAttributeT<double> {}
{}

SolverDefinitionGridAttributeReal::SolverDefinitionGridAttributeReal(const QDomElement& elem, SolverDefinition* solverDef, Position pos, bool isOption, int order) :
	SolverDefinitionGridAttributeT<double> (elem, solverDef, pos, isOption, order)
{}

GridAttributeEditWidget* SolverDefinitionGridAttributeReal::editWidget(QWidget* parent)
{
	return new GridAttributeRealEditWidget(parent, this);
}

GridAttributeVariationEditWidget* SolverDefinitionGridAttributeReal::variationEditWidget(QWidget* parent)
{
	return new GridAttributeRealVariationEditWidget(parent, this);
}

GridAttributeContainer* SolverDefinitionGridAttributeReal::buildContainer(v4InputGrid* grid)
{
	return new GridAttributeRealContainer(grid, this);
}

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
