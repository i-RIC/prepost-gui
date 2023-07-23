#include "../pre/gridcond/editwidget/gridattributerealoptioneditwidget.h"
#include "../pre/gridcond/stringconverter/gridattributestringconverterenumerate.h"
#include "../scalarstocolors/colormapenumeratefactory.h"
#include "../scalarstocolors/colormapenumeratesettingcontainer.h"
#include "solverdefinition.h"
#include "solverdefinitiongridattributerealoptioniface.h"

SolverDefinitionGridAttributeRealOptionIFace::SolverDefinitionGridAttributeRealOptionIFace(const QDomElement& elem, SolverDefinition* solverDef, int order) :
	SolverDefinitionGridAttributeRealIFace {elem, solverDef, true, order}
{
	loadEnumeration(elem, solverDef->buildTranslator());
	setColorMapFactory(new ColorMapEnumerateFactory());
}

GridAttributeVariationEditWidget* SolverDefinitionGridAttributeRealOptionIFace::variationEditWidget(QWidget*)
{
	return nullptr;
}

GridAttributeStringConverter* SolverDefinitionGridAttributeRealOptionIFace::stringConverter() const
{
	auto ret = new GridAttributeStringConverterEnumerate();
	ret->setEnumerations(variantEnumerations());

	return ret;
}

GridAttributeEditWidget* SolverDefinitionGridAttributeRealOptionIFace::editWidget(QWidget* parent)
{
	GridAttributeRealOptionEditWidget* w = new GridAttributeRealOptionEditWidget(parent, this);
	w->setEnumerations(enumerations());
	return w;
}

void SolverDefinitionGridAttributeRealOptionIFace::setupColorMapSettingContainer(ColorMapSettingContainerI* c) const
{
	auto cont = dynamic_cast<ColorMapEnumerateSettingContainer*> (c);
	cont->setup(this);
}
