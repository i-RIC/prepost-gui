#include "../pre/gridcond/editwidget/gridattributeintegeroptioneditwidget.h"
#include "../scalarstocolors/colormapenumeratefactory.h"
#include "../scalarstocolors/colormapenumeratesettingcontainer.h"
#include "solverdefinition.h"
#include "solverdefinitiongridattributeintegeroption.h"

SolverDefinitionGridAttributeIntegerOption::SolverDefinitionGridAttributeIntegerOption(const QDomElement& elem, SolverDefinition* solverDef, Position pos, int order) :
	SolverDefinitionGridAttributeInteger(elem, solverDef, pos, true, order)
{
	loadEnumeration(elem, solverDef->buildTranslator());
	setColorMapFactory(new ColorMapEnumerateFactory());
}

GridAttributeEditWidget* SolverDefinitionGridAttributeIntegerOption::editWidget(QWidget* parent)
{
	auto w = new GridAttributeIntegerOptionEditWidget(parent, this);
	w->setEnumerations(enumerations());
	return w;
}

GridAttributeVariationEditWidget* SolverDefinitionGridAttributeIntegerOption::variationEditWidget(QWidget* /*parent*/)
{
	return nullptr;
}

void SolverDefinitionGridAttributeIntegerOption::setupColorMapSettingContainer(ColorMapSettingContainerI* c) const
{
	auto cont = dynamic_cast<ColorMapEnumerateSettingContainer*> (c);
	cont->setup(this);
}
