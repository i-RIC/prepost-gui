#include "../pre/gridcond/editwidget/gridattributeintegeroptioneditwidget.h"
#include "../pre/gridcond/stringconverter/gridattributestringconverterenumerate.h"
#include "../project/colorsource.h"
#include "../scalarstocolors/colormapenumeratefactory.h"
#include "../scalarstocolors/colormapenumeratesettingcontainer.h"
#include "solverdefinition.h"
#include "solverdefinitiongridattributeintegeroptionjface.h"

#include <vtkColorTransferFunction.h>

SolverDefinitionGridAttributeIntegerOptionJFace::SolverDefinitionGridAttributeIntegerOptionJFace(const QDomElement& elem, SolverDefinition *solverDef, int order) :
	SolverDefinitionGridAttributeIntegerJFace(elem, solverDef, true, order)
{
	loadEnumeration(elem, solverDef->buildTranslator());
	setColorMapFactory(new ColorMapEnumerateFactory());
}

GridAttributeStringConverter* SolverDefinitionGridAttributeIntegerOptionJFace::stringConverter() const
{
	auto ret = new GridAttributeStringConverterEnumerate();
	ret->setEnumerations(variantEnumerations());

	return ret;
}

GridAttributeEditWidget* SolverDefinitionGridAttributeIntegerOptionJFace::editWidget(QWidget* parent)
{
	GridAttributeIntegerOptionEditWidget* w = new GridAttributeIntegerOptionEditWidget(parent, this);
	w->setEnumerations(enumerations());
	return w;
}

GridAttributeVariationEditWidget* SolverDefinitionGridAttributeIntegerOptionJFace::variationEditWidget(QWidget*)
{
	return nullptr;
}

void SolverDefinitionGridAttributeIntegerOptionJFace::setupColorMapSettingContainer(ColorMapSettingContainerI* c) const
{
	auto cont = dynamic_cast<ColorMapEnumerateSettingContainer*> (c);
	cont->setup(this);
}
