#ifndef SOLVERDEFINITIONREALOPTIONNODEGRIDRELATEDCONDITION_H
#define SOLVERDEFINITIONREALOPTIONNODEGRIDRELATEDCONDITION_H

#include "solverdefinitiongridattributerealnode.h"
#include "enumloader.h"

class SolverDefinitionGridAttributeRealOptionNode : public SolverDefinitionGridAttributeRealNode, public RealEnumLoader
{

public:
	SolverDefinitionGridAttributeRealOptionNode(QDomElement node, const SolverDefinitionTranslator& translator);
	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* /*parent*/) override {return 0;}
	ScalarsToColorsEditWidget* createScalarsToColorsEditWidget(QWidget* parent) override;
	ScalarsToColorsContainer* createScalarsToColorsContainer(ProjectDataItem* d) override;
};

#endif // SOLVERDEFINITIONREALOPTIONNODEGRIDRELATEDCONDITION_H
