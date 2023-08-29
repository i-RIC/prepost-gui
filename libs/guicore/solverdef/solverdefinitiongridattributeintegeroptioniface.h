#ifndef SOLVERDEFINITIONGRIDATTRIBUTEINTEGEROPTIONIFACE_H
#define SOLVERDEFINITIONGRIDATTRIBUTEINTEGEROPTIONIFACE_H

#include "solverdefinitiongridattributeintegeriface.h"
#include "integerenumloader.h"

class SolverDefinitionGridAttributeIntegerOptionIFace : public SolverDefinitionGridAttributeIntegerIFace, public IntegerEnumLoader
{
public:
	SolverDefinitionGridAttributeIntegerOptionIFace(const QDomElement& elem, SolverDefinition* solverDef, int order);

	GridAttributeStringConverter* stringConverter() const override;
	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;

private:
	void setupColorMapSettingContainer(ColorMapSettingContainerI* c) const override;
};

#endif // SOLVERDEFINITIONGRIDATTRIBUTEINTEGEROPTIONIFACE_H
