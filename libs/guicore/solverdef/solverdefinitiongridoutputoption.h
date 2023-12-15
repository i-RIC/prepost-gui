#ifndef SOLVERDEFINITIONGRIDOUTPUTOPTION_H
#define SOLVERDEFINITIONGRIDOUTPUTOPTION_H

#include "../guicore_global.h"
#include "realenumloader.h"
#include "solverdefinitiongridoutput.h"

class SolverDefinitionGridOutputOption : public SolverDefinitionGridOutput, public RealEnumLoader
{
public:
	SolverDefinitionGridOutputOption(const QDomElement& elem, SolverDefinition* solverDef, const SolverDefinitionTranslator& translator);

	QVariant attributeBrowserValue(double value) const override;
	GridAttributeStringConverter* stringConverter() const override;

private:
	void setupColorMapSettingContainer(ColorMapSettingContainerI* c) const override;
};

#endif // SOLVERDEFINITIONGRIDOUTPUTOPTION_H
