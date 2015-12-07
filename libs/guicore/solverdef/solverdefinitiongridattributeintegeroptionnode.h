#ifndef SOLVERDEFINITIONINTEGEROPTIONNODEGRIDATTRIBUTE_H
#define SOLVERDEFINITIONINTEGEROPTIONNODEGRIDATTRIBUTE_H

#include "solverdefinitiongridattributeintegernode.h"
#include "enumloader.h"

class SolverDefinitionGridAttributeIntegerOptionNode : public SolverDefinitionGridAttributeIntegerNode, public IntegerEnumLoader
{

public:
	SolverDefinitionGridAttributeIntegerOptionNode(QDomElement node, const SolverDefinitionTranslator& translator, int order);
	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* /*parent*/) override {return 0;}
	ScalarsToColorsEditWidget* createScalarsToColorsEditWidget(QWidget* parent) const override;
	ScalarsToColorsContainer* createScalarsToColorsContainer(ProjectDataItem* d) override;
};

#endif // SOLVERDEFINITIONINTEGEROPTIONNODEGRIDATTRIBUTE_H
