#ifndef SOLVERDEFINITIONGRIDATTRIBUTEREALOPTIONNODE_H
#define SOLVERDEFINITIONGRIDATTRIBUTEREALOPTIONNODE_H

#include "solverdefinitiongridattributerealnode.h"
#include "realenumloader.h"

class SolverDefinitionGridAttributeRealOptionNode : public SolverDefinitionGridAttributeRealNode, public RealEnumLoader
{
public:
	SolverDefinitionGridAttributeRealOptionNode(QDomElement node, SolverDefinition* solverDef, int order);

	GridAttributeStringConverter* stringConverter() const override;
	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;

	ScalarsToColorsEditWidget* createScalarsToColorsEditWidget(QWidget* parent) const override;
	ScalarsToColorsContainer* createScalarsToColorsContainer(ProjectDataItem* d) override;
};

#endif // SOLVERDEFINITIONGRIDATTRIBUTEREALOPTIONNODE_H
