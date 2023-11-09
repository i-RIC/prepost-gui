#include "../scalarstocolors/colormapenumeratefactory.h"
#include "../scalarstocolors/colormapenumeratesettingcontainer.h"
#include "solverdefinitiongridoutputoption.h"

SolverDefinitionGridOutputOption::SolverDefinitionGridOutputOption(const QDomElement& elem, SolverDefinition* solverDef, const SolverDefinitionTranslator& translator) :
	SolverDefinitionGridOutput(elem, solverDef, translator),
	RealEnumLoader {}
{
	loadEnumeration(elem, translator);
	setColorMapFactory(new ColorMapEnumerateFactory());

	auto s = iRIC::getChildNode(elem, "EnumerateColorMapSetting");
	if (! s.isNull()) {
		setDefaultColorMapSetting(s.toElement());
	}
}

QVariant SolverDefinitionGridOutputOption::attributeBrowserValue(double value) const
{
	auto intValue = static_cast<int> (value);
	return enumerations().value(intValue, "");
}

void SolverDefinitionGridOutputOption::setupColorMapSettingContainer(ColorMapSettingContainerI* c) const
{
	auto c2 = dynamic_cast<ColorMapEnumerateSettingContainer*> (c);
	c2->setup(this);
}
