#ifndef SOLVERDEFINITIONGRIDRELATEDCONDITIONDIMENSION_H
#define SOLVERDEFINITIONGRIDRELATEDCONDITIONDIMENSION_H

#include "../guicore_global.h"
#include "solverdefinitionnode.h"

class SolverDefinitionGridRelatedCondition;
class GridRelatedConditionDimensionContainer;
class GridRelatedConditionDimensionSelectWidget;

class GUICOREDLL_EXPORT SolverDefinitionGridRelatedConditionDimension : public SolverDefinitionNode
{
public:
	SolverDefinitionGridRelatedConditionDimension(QDomElement node, const SolverDefinitionTranslator& translator, SolverDefinitionGridRelatedCondition* parent)
		: SolverDefinitionNode(node, translator) {
		m_condition = parent;
		load(node, translator);
	}
	virtual ~SolverDefinitionGridRelatedConditionDimension();
	const QString& name() const {return m_name;}
	const QString& caption() const {return m_caption;}
	const QString& englishCaption() const {return m_englishCaption;}
	SolverDefinitionGridRelatedCondition* condition() const {return m_condition;}
	virtual GridRelatedConditionDimensionContainer* buildContainer() = 0;
	GridRelatedConditionDimensionSelectWidget* buildSelectWidget(GridRelatedConditionDimensionContainer* container);

protected:
	/// Load settings from solver definition file.
	void load(const QDomElement& node, const SolverDefinitionTranslator& translator);

	QString m_name;
	QString m_caption;
	QString m_englishCaption;

	SolverDefinitionGridRelatedCondition* m_condition;
};

#endif // SOLVERDEFINITIONGRIDRELATEDCONDITIONDIMENSION_H
