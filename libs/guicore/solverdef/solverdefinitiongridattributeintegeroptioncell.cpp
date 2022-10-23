#include "../pre/gridcond/editwidget/gridattributeintegeroptioneditwidget.h"
#include "../pre/gridcond/stringconverter/gridattributestringconverterenumerate.h"
#include "../project/colorsource.h"
#include "../scalarstocolors/colormapenumeratefactory.h"
#include "../scalarstocolors/colormapenumeratesettingcontainer.h"
#include "solverdefinition.h"
#include "solverdefinitiongridattributeintegeroptioncell.h"

#include <vtkColorTransferFunction.h>

SolverDefinitionGridAttributeIntegerOptionCell::SolverDefinitionGridAttributeIntegerOptionCell(const QDomElement& elem, SolverDefinition *solverDef, int order) :
	SolverDefinitionGridAttributeIntegerCell(elem, solverDef, true, order)
{
	loadEnumeration(elem, solverDef->buildTranslator());
	setColorMapFactory(new ColorMapEnumerateFactory());
}

GridAttributeStringConverter* SolverDefinitionGridAttributeIntegerOptionCell::stringConverter() const
{
	auto ret = new GridAttributeStringConverterEnumerate();
	ret->setEnumerations(variantEnumerations());

	return ret;
}

GridAttributeEditWidget* SolverDefinitionGridAttributeIntegerOptionCell::editWidget(QWidget* parent)
{
	GridAttributeIntegerOptionEditWidget* w = new GridAttributeIntegerOptionEditWidget(parent, this);
	w->setEnumerations(enumerations());
	return w;
}

GridAttributeVariationEditWidget* SolverDefinitionGridAttributeIntegerOptionCell::variationEditWidget(QWidget*)
{
	return nullptr;
}

void SolverDefinitionGridAttributeIntegerOptionCell::setupColorMapSettingContainer(ColorMapSettingContainerI* c) const
{
	auto cont = dynamic_cast<ColorMapEnumerateSettingContainer*> (c);
	cont->setup(this);
}
