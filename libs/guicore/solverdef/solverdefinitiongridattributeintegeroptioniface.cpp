#include "../pre/gridcond/editwidget/gridattributeintegeroptioneditwidget.h"
#include "../pre/gridcond/stringconverter/gridattributestringconverterenumerate.h"
#include "../project/colorsource.h"
#include "../scalarstocolors/colormapenumeratefactory.h"
#include "../scalarstocolors/colormapenumeratesettingcontainer.h"
#include "solverdefinition.h"
#include "solverdefinitiongridattributeintegeroptioniface.h"

#include <vtkColorTransferFunction.h>

SolverDefinitionGridAttributeIntegerOptionIFace::SolverDefinitionGridAttributeIntegerOptionIFace(const QDomElement& elem, SolverDefinition *solverDef, int order) :
	SolverDefinitionGridAttributeIntegerIFace(elem, solverDef, true, order)
{
	loadEnumeration(elem, solverDef->buildTranslator());
	setColorMapFactory(new ColorMapEnumerateFactory());
}

GridAttributeStringConverter* SolverDefinitionGridAttributeIntegerOptionIFace::stringConverter() const
{
	auto ret = new GridAttributeStringConverterEnumerate();
	ret->setEnumerations(variantEnumerations());

	return ret;
}

GridAttributeEditWidget* SolverDefinitionGridAttributeIntegerOptionIFace::editWidget(QWidget* parent)
{
	GridAttributeIntegerOptionEditWidget* w = new GridAttributeIntegerOptionEditWidget(parent, this);
	w->setEnumerations(enumerations());
	return w;
}

GridAttributeVariationEditWidget* SolverDefinitionGridAttributeIntegerOptionIFace::variationEditWidget(QWidget*)
{
	return nullptr;
}

void SolverDefinitionGridAttributeIntegerOptionIFace::setupColorMapSettingContainer(ColorMapSettingContainerI* c) const
{
	auto cont = dynamic_cast<ColorMapEnumerateSettingContainer*> (c);
	cont->setup(this);
}
