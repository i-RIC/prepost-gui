#ifndef SOLVERDEFINITIONINTEGEROPTIONCELLGRIDRELATEDCONDITION_H
#define SOLVERDEFINITIONINTEGEROPTIONCELLGRIDRELATEDCONDITION_H

#include "solverdefinitiongridattributeintegercell.h"
#include "enumloader.h"

class SolverDefinitionGridAttributeIntegerOptionCell : public SolverDefinitionGridAttributeIntegerCell, public IntegerEnumLoader
{

public:
	SolverDefinitionGridAttributeIntegerOptionCell(QDomElement node, const SolverDefinitionTranslator& translator, int order);
	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* /*parent*/) override {return 0;}
	ScalarsToColorsEditWidget* createScalarsToColorsEditWidget(QWidget* parent) const override;
	ScalarsToColorsContainer* createScalarsToColorsContainer(ProjectDataItem* d) override;
};

#endif // SOLVERDEFINITIONINTEGEROPTIONCELLGRIDRELATEDCONDITION_H
