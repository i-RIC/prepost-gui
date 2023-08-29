#ifndef SOLVERDEFINITIONGRIDATTRIBUTEINTEGEROPTIONJFACE_H
#define SOLVERDEFINITIONGRIDATTRIBUTEINTEGEROPTIONJFACE_H

#include "solverdefinitiongridattributeintegerjface.h"
#include "integerenumloader.h"

class SolverDefinitionGridAttributeIntegerOptionJFace : public SolverDefinitionGridAttributeIntegerJFace, public IntegerEnumLoader
{
public:
	SolverDefinitionGridAttributeIntegerOptionJFace(const QDomElement& elem, SolverDefinition* solverDef, int order);

	GridAttributeStringConverter* stringConverter() const override;
	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;

private:
	void setupColorMapSettingContainer(ColorMapSettingContainerI* c) const override;
};

#endif // SOLVERDEFINITIONGRIDATTRIBUTEINTEGEROPTIONJFACE_H
