#ifndef SOLVERDEFINITIONINTEGEROPTIONNODEGRIDATTRIBUTE_H
#define SOLVERDEFINITIONINTEGEROPTIONNODEGRIDATTRIBUTE_H

#include "solverdefinitiongridattributeintegernode.h"
#include "integerenumloader.h"

class SolverDefinitionGridAttributeIntegerOptionNode : public SolverDefinitionGridAttributeIntegerNode, public IntegerEnumLoader
{
public:
	SolverDefinitionGridAttributeIntegerOptionNode(const QDomElement& elem, SolverDefinition* solverDef, int order);

	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;

	ScalarsToColorsEditWidget* createScalarsToColorsEditWidget(QWidget* parent) const override;
	ScalarsToColorsContainer* createScalarsToColorsContainer(ProjectDataItem* d) override;
};

#endif // SOLVERDEFINITIONINTEGEROPTIONNODEGRIDATTRIBUTE_H
