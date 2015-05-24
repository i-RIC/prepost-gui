#ifndef SOLVERDEFINITIONGRIDRELATEDCOMPLEXCONDITION_H
#define SOLVERDEFINITIONGRIDRELATEDCOMPLEXCONDITION_H

#include "../guicore_global.h"
#include "solverdefinitionnode.h"
#include "solverdefinitiongridrelatedconditiont.h"
#include <QString>
#include <QDomElement>

class GridRelatedConditionContainer;
class SolverDefinition;
class PreProcessorDataItem;
class PreProcessorGridRelatedComplexConditionDataItem;

class GUICOREDLL_EXPORT SolverDefinitionGridRelatedComplexCondition : public SolverDefinitionGridRelatedIntegerCondition
{
public:
	SolverDefinitionGridRelatedComplexCondition(QDomElement node, const SolverDefinitionTranslator& translator)
		: SolverDefinitionGridRelatedIntegerCondition(node, translator) {
		load(node);
	}
	~SolverDefinitionGridRelatedComplexCondition() {}
	const QDomElement& element() {return m_element;}
	GridRelatedConditionContainer* container(Grid* grid);

	GridRelatedConditionEditWidget* editWidget(QWidget* parent);
	GridRelatedConditionVariationEditWidget* variationEditWidget(QWidget* /*parent*/) {return 0;}
	RawData* buildBackgroundRawData(ProjectDataItem* parent) override;
	ScalarsToColorsContainer* createScalarsToColorsContainer(ProjectDataItem* d) override;
	ScalarsToColorsEditWidget* createScalarsToColorsEditWidget(QWidget* parent) override;
	QString undefinedString();
	QString undefinedEnglishString();

private:
	void load(const QDomElement node);
	GridRelatedConditionContainer* buildContainer(Grid* grid) override;
	QDomElement m_element;
	SolverDefinition* m_definition;
};

#endif // SOLVERDEFINITIONGRIDRELATEDCOMPLEXCONDITION_H
