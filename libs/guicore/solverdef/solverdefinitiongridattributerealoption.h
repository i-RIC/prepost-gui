#ifndef SOLVERDEFINITIONGRIDATTRIBUTEREALOPTION_H
#define SOLVERDEFINITIONGRIDATTRIBUTEREALOPTION_H

#include "../guicore_global.h"

#include "realenumloader.h"
#include "solverdefinitiongridattributereal.h"

class SolverDefinitionGridAttributeRealOption : public SolverDefinitionGridAttributeReal, public RealEnumLoader
{
public:
	SolverDefinitionGridAttributeRealOption(const QDomElement& elem, SolverDefinition* solverDef, Position pos, int order);

	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;
	GridAttributeStringConverter* stringConverter() const override;

private:
	void setupColorMapSettingContainer(ColorMapSettingContainerI* c) const override;
};

#endif // SOLVERDEFINITIONGRIDATTRIBUTEREALOPTION_H
