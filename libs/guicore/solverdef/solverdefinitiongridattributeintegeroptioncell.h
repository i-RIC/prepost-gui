#ifndef SOLVERDEFINITIONINTEGEROPTIONCELLGRIDATTRIBUTE_H
#define SOLVERDEFINITIONINTEGEROPTIONCELLGRIDATTRIBUTE_H

#include "solverdefinitiongridattributeintegercell.h"
#include "integerenumloader.h"

class SolverDefinitionGridAttributeIntegerOptionCell : public SolverDefinitionGridAttributeIntegerCell, public IntegerEnumLoader
{
public:
	SolverDefinitionGridAttributeIntegerOptionCell(const QDomElement& elem, SolverDefinition* solverDef, int order);

	GridAttributeStringConverter* stringConverter() const override;
	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;

private:
	void setupColorMapSettingContainer(ColorMapSettingContainerI* c) const override;
};

#endif // SOLVERDEFINITIONINTEGEROPTIONCELLGRIDATTRIBUTE_H
