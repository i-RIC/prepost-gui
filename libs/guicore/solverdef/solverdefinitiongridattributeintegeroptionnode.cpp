#include "../pre/gridcond/editwidget/gridattributeintegeroptioneditwidget.h"
#include "../project/colorsource.h"
#include "../scalarstocolors/colormapenumeratefactory.h"
#include "../scalarstocolors/colormapenumeratesettingcontainer.h"
#include "solverdefinition.h"
#include "solverdefinitiongridattributeintegeroptionnode.h"

SolverDefinitionGridAttributeIntegerOptionNode::SolverDefinitionGridAttributeIntegerOptionNode(const QDomElement& elem, SolverDefinition* solverDef, int order) :
	SolverDefinitionGridAttributeIntegerNode {elem, solverDef, true, order}
{
	loadEnumeration(elem, solverDef->buildTranslator());
	setColorMapFactory(new ColorMapEnumerateFactory());
}

GridAttributeEditWidget* SolverDefinitionGridAttributeIntegerOptionNode::editWidget(QWidget* parent)
{
	GridAttributeIntegerOptionEditWidget* w = new GridAttributeIntegerOptionEditWidget(parent, this);
	w->setEnumerations(enumerations());
	return w;
}

GridAttributeVariationEditWidget* SolverDefinitionGridAttributeIntegerOptionNode::variationEditWidget(QWidget*)
{
	return nullptr;
}

void SolverDefinitionGridAttributeIntegerOptionNode::setupColorMapSettingContainer(ColorMapSettingContainerI* c) const
{
	auto cont = dynamic_cast<ColorMapEnumerateSettingContainer*> (c);
	cont->setup(this);
}
