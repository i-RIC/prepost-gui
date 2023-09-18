#ifndef SOLVERDEFINITIONGRIDATTRIBUTEINTEGEROPTION_H
#define SOLVERDEFINITIONGRIDATTRIBUTEINTEGEROPTION_H

#include "../guicore_global.h"

#include "integerenumloader.h"
#include "solverdefinitiongridattributeinteger.h"

class SolverDefinitionGridAttributeIntegerOption : public SolverDefinitionGridAttributeInteger, public IntegerEnumLoader
{
public:
	SolverDefinitionGridAttributeIntegerOption(const QDomElement& elem, SolverDefinition* solverDef, Position pos, int order);

	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;

private:
	void setupColorMapSettingContainer(ColorMapSettingContainerI* c) const override;
};

#endif // SOLVERDEFINITIONGRIDATTRIBUTEINTEGEROPTION_H
