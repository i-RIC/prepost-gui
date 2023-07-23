#ifndef SOLVERDEFINITIONGRIDATTRIBUTEREALOPTIONJFACE_H
#define SOLVERDEFINITIONGRIDATTRIBUTEREALOPTIONJFACE_H

#include "solverdefinitiongridattributerealjface.h"
#include "realenumloader.h"

class SolverDefinitionGridAttributeRealOptionJFace : public SolverDefinitionGridAttributeRealJFace, public RealEnumLoader
{
public:
	SolverDefinitionGridAttributeRealOptionJFace(const QDomElement& elem, SolverDefinition* solverDef, int order);

	GridAttributeStringConverter* stringConverter() const override;
	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;

private:
	void setupColorMapSettingContainer(ColorMapSettingContainerI* c) const override;
};

#endif // SOLVERDEFINITIONGRIDATTRIBUTEREALOPTIONJFACE_H
