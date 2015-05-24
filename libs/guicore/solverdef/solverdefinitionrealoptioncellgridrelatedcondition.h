#ifndef SOLVERDEFINITIONREALOPTIONCELLGRIDRELATEDCONDITION_H
#define SOLVERDEFINITIONREALOPTIONCELLGRIDRELATEDCONDITION_H

#include "solverdefinitionrealcellgridrelatedcondition.h"
#include "enumloader.h"

class SolverDefinitionRealOptionCellGridRelatedCondition : public SolverDefinitionRealCellGridRelatedCondition, public RealEnumLoader
{
public:
	SolverDefinitionRealOptionCellGridRelatedCondition(QDomElement node, const SolverDefinitionTranslator& translator);
	GridRelatedConditionEditWidget* editWidget(QWidget* parent);
	GridRelatedConditionVariationEditWidget* variationEditWidget(QWidget* /*parent*/) {return 0;}
	ScalarsToColorsEditWidget* createScalarsToColorsEditWidget(QWidget* parent) override;
	ScalarsToColorsContainer* createScalarsToColorsContainer(ProjectDataItem* d) override;
};

#endif // SOLVERDEFINITIONREALOPTIONCELLGRIDRELATEDCONDITION_H
