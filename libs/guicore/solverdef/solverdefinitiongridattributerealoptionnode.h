#ifndef SOLVERDEFINITIONGRIDATTRIBUTEREALOPTIONNODE_H
#define SOLVERDEFINITIONGRIDATTRIBUTEREALOPTIONNODE_H

#include "solverdefinitiongridattributerealnode.h"
#include "enumloader.h"

class SolverDefinitionGridAttributeRealOptionNode : public SolverDefinitionGridAttributeRealNode, public RealEnumLoader
{
public:
	SolverDefinitionGridAttributeRealOptionNode(QDomElement node, const SolverDefinitionTranslator& translator, int order);

	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;

	ScalarsToColorsEditWidget* createScalarsToColorsEditWidget(QWidget* parent) const override;
	ScalarsToColorsContainer* createScalarsToColorsContainer(ProjectDataItem* d) override;
};

#endif // SOLVERDEFINITIONGRIDATTRIBUTEREALOPTIONNODE_H
