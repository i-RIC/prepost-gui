#ifndef SOLVERDEFINITIONREALOPTIONCELLGRIDRELATEDCONDITION_H
#define SOLVERDEFINITIONREALOPTIONCELLGRIDRELATEDCONDITION_H

#include "solverdefinitiongridattributerealcell.h"
#include "enumloader.h"

class SolverDefinitionGridAttributeRealOptionCell : public SolverDefinitionGridAttributeRealCell, public RealEnumLoader
{

public:
	SolverDefinitionGridAttributeRealOptionCell(QDomElement node, const SolverDefinitionTranslator& translator);
	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* /*parent*/) override {return 0;}
	ScalarsToColorsEditWidget* createScalarsToColorsEditWidget(QWidget* parent) const override;
	ScalarsToColorsContainer* createScalarsToColorsContainer(ProjectDataItem* d) override;
};

#endif // SOLVERDEFINITIONREALOPTIONCELLGRIDRELATEDCONDITION_H
