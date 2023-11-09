#include "../pre/gridcond/editwidget/gridattributerealoptioneditwidget.h"
#include "../scalarstocolors/colormapenumeratefactory.h"
#include "../scalarstocolors/colormapenumeratesettingcontainer.h"
#include "solverdefinition.h"
#include "solverdefinitiongridattributerealoption.h"

SolverDefinitionGridAttributeRealOption::SolverDefinitionGridAttributeRealOption(const QDomElement& elem, SolverDefinition* solverDef, Position pos, int order) :
	SolverDefinitionGridAttributeReal(elem, solverDef, pos, true, order)
{
	loadEnumeration(elem, solverDef->buildTranslator());
	setColorMapFactory(new ColorMapEnumerateFactory());
}

GridAttributeEditWidget* SolverDefinitionGridAttributeRealOption::editWidget(QWidget* parent)
{
	auto w = new GridAttributeRealOptionEditWidget(parent, this);
	w->setEnumerations(enumerations());
	return w;
}

GridAttributeVariationEditWidget* SolverDefinitionGridAttributeRealOption::variationEditWidget(QWidget* /*parent*/)
{
	return nullptr;
}

void SolverDefinitionGridAttributeRealOption::setupColorMapSettingContainer(ColorMapSettingContainerI* c) const
{
	auto cont = dynamic_cast<ColorMapEnumerateSettingContainer*> (c);
	cont->setup(this);
}
