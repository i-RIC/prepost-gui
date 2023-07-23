#include "../pre/gridcond/editwidget/gridattributerealoptioneditwidget.h"
#include "../pre/gridcond/stringconverter/gridattributestringconverterenumerate.h"
#include "../scalarstocolors/colormapenumeratefactory.h"
#include "../scalarstocolors/colormapenumeratesettingcontainer.h"
#include "solverdefinition.h"
#include "solverdefinitiongridattributerealoptionjface.h"

SolverDefinitionGridAttributeRealOptionJFace::SolverDefinitionGridAttributeRealOptionJFace(const QDomElement& elem, SolverDefinition* solverDef, int order) :
	SolverDefinitionGridAttributeRealJFace {elem, solverDef, true, order}
{
	loadEnumeration(elem, solverDef->buildTranslator());
	setColorMapFactory(new ColorMapEnumerateFactory());
}

GridAttributeVariationEditWidget* SolverDefinitionGridAttributeRealOptionJFace::variationEditWidget(QWidget*)
{
	return nullptr;
}

GridAttributeStringConverter* SolverDefinitionGridAttributeRealOptionJFace::stringConverter() const
{
	auto ret = new GridAttributeStringConverterEnumerate();
	ret->setEnumerations(variantEnumerations());

	return ret;
}

GridAttributeEditWidget* SolverDefinitionGridAttributeRealOptionJFace::editWidget(QWidget* parent)
{
	GridAttributeRealOptionEditWidget* w = new GridAttributeRealOptionEditWidget(parent, this);
	w->setEnumerations(enumerations());
	return w;
}

void SolverDefinitionGridAttributeRealOptionJFace::setupColorMapSettingContainer(ColorMapSettingContainerI* c) const
{
	auto cont = dynamic_cast<ColorMapEnumerateSettingContainer*> (c);
	cont->setup(this);
}
