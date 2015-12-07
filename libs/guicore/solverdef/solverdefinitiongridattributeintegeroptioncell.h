#ifndef SOLVERDEFINITIONINTEGEROPTIONCELLGRIDATTRIBUTE_H
#define SOLVERDEFINITIONINTEGEROPTIONCELLGRIDATTRIBUTE_H

#include "solverdefinitiongridattributeintegercell.h"
#include "integerenumloader.h"

class SolverDefinitionGridAttributeIntegerOptionCell : public SolverDefinitionGridAttributeIntegerCell, public IntegerEnumLoader
{
public:
	SolverDefinitionGridAttributeIntegerOptionCell(const QDomElement& elem, const SolverDefinitionTranslator& translator, int order);

	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;

	ScalarsToColorsEditWidget* createScalarsToColorsEditWidget(QWidget* parent) const override;
	ScalarsToColorsContainer* createScalarsToColorsContainer(ProjectDataItem* d) override;
};

#endif // SOLVERDEFINITIONINTEGEROPTIONCELLGRIDATTRIBUTE_H
