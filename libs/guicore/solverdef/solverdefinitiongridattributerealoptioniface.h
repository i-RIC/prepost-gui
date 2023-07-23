#ifndef SOLVERDEFINITIONGRIDATTRIBUTEREALOPTIONIFACE_H
#define SOLVERDEFINITIONGRIDATTRIBUTEREALOPTIONIFACE_H

#include "solverdefinitiongridattributerealiface.h"
#include "realenumloader.h"

class SolverDefinitionGridAttributeRealOptionIFace : public SolverDefinitionGridAttributeRealIFace, public RealEnumLoader
{
public:
	SolverDefinitionGridAttributeRealOptionIFace(const QDomElement& elem, SolverDefinition* solverDef, int order);

	GridAttributeStringConverter* stringConverter() const override;
	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;

private:
	void setupColorMapSettingContainer(ColorMapSettingContainerI* c) const override;
};

#endif // SOLVERDEFINITIONGRIDATTRIBUTEREALOPTIONIFACE_H
