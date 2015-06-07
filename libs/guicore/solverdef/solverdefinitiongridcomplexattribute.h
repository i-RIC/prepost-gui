#ifndef SOLVERDEFINITIONGRIDRELATEDCOMPLEXCONDITION_H
#define SOLVERDEFINITIONGRIDRELATEDCOMPLEXCONDITION_H

#include "../guicore_global.h"
#include "solverdefinitionnode.h"
#include "solverdefinitiongridattributet.h"
#include <QString>
#include <QDomElement>

class GridAttributeContainer;
class SolverDefinition;
class PreProcessorDataItem;
class PreProcessorGridRelatedComplexConditionDataItem;

class GUICOREDLL_EXPORT SolverDefinitionGridComplexAttribute : public SolverDefinitionGridAttributeInteger
{
public:
	SolverDefinitionGridComplexAttribute(QDomElement node, const SolverDefinitionTranslator& translator)
		: SolverDefinitionGridAttributeInteger(node, translator) {
		load(node);
	}
	~SolverDefinitionGridComplexAttribute() {}
	const QDomElement& element() {return m_element;}
	GridAttributeContainer* container(Grid* grid);

	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* /*parent*/) override {return 0;}
	RawData* buildBackgroundRawData(ProjectDataItem* parent) override;
	ScalarsToColorsContainer* createScalarsToColorsContainer(ProjectDataItem* d) override;
	ScalarsToColorsEditWidget* createScalarsToColorsEditWidget(QWidget* parent) override;
	QString undefinedString();
	QString undefinedEnglishString();

private:
	void load(const QDomElement node);
	GridAttributeContainer* buildContainer(Grid* grid) override;
	QDomElement m_element;
	SolverDefinition* m_definition;
};

#endif // SOLVERDEFINITIONGRIDRELATEDCOMPLEXCONDITION_H
