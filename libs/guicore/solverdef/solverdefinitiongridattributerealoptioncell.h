#ifndef SOLVERDEFINITIONGRIDATTRIBUTEREALOPTIONCELL_H
#define SOLVERDEFINITIONGRIDATTRIBUTEREALOPTIONCELL_H

#include "solverdefinitiongridattributerealcell.h"
#include "realenumloader.h"

class SolverDefinitionGridAttributeRealOptionCell : public SolverDefinitionGridAttributeRealCell, public RealEnumLoader
{
public:
	SolverDefinitionGridAttributeRealOptionCell(const QDomElement& elem, const SolverDefinitionTranslator& translator, int order);

	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;

	ScalarsToColorsEditWidget* createScalarsToColorsEditWidget(QWidget* parent) const override;
	ScalarsToColorsContainer* createScalarsToColorsContainer(ProjectDataItem* d) override;
};

#endif // SOLVERDEFINITIONGRIDATTRIBUTEREALOPTIONCELL_H
