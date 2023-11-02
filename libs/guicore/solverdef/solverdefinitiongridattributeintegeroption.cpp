#include "../pre/gridcond/editwidget/gridattributeintegeroptioneditwidget.h"
#include "../pre/gridcond/stringconverter/gridattributestringconverterenumerate.h"
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

GridAttributeStringConverter* SolverDefinitionGridAttributeIntegerOption::stringConverter() const
{
	auto converter = new GridAttributeStringConverterEnumerate();
	auto origEnums = enumerations();
	QMap<QVariant, QString> enums;
	for (auto it = origEnums.begin(); it != origEnums.end(); ++it) {
		enums.insert(QVariant(it.key()), it.value());
	}
	converter->setEnumerations(enums);
	return converter;
}

void SolverDefinitionGridAttributeIntegerOption::setupColorMapSettingContainer(ColorMapSettingContainerI* c) const
{
	auto cont = dynamic_cast<ColorMapEnumerateSettingContainer*> (c);
	cont->setup(this);
}
