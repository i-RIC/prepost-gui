#ifndef SOLVERDEFINITIONGRIDRELATEDCONDITIONDIMENSION_H
#define SOLVERDEFINITIONGRIDRELATEDCONDITIONDIMENSION_H

#include "../guicore_global.h"
#include "solverdefinitionnode.h"

class SolverDefinitionGridAttribute;
class GridAttributeDimensionContainer;
class GridAttributeDimensionSelectWidget;

class GUICOREDLL_EXPORT SolverDefinitionGridAttributeDimension : public SolverDefinitionNode
{

public:
	SolverDefinitionGridAttributeDimension(QDomElement node, const SolverDefinitionTranslator& translator, SolverDefinitionGridAttribute* parent)
		: SolverDefinitionNode(node, translator) {
		m_condition = parent;
		load(node, translator);
	}
	virtual ~SolverDefinitionGridAttributeDimension();
	const QString& name() const {return m_name;}
	const QString& caption() const {return m_caption;}
	const QString& englishCaption() const {return m_englishCaption;}
	SolverDefinitionGridAttribute* condition() const {return m_condition;}
	virtual GridAttributeDimensionContainer* buildContainer() = 0;
	GridAttributeDimensionSelectWidget* buildSelectWidget(GridAttributeDimensionContainer* container);

protected:
	/// Load settings from solver definition file.
	void load(const QDomElement& node, const SolverDefinitionTranslator& translator);

	QString m_name;
	QString m_caption;
	QString m_englishCaption;

	SolverDefinitionGridAttribute* m_condition;
};

#endif // SOLVERDEFINITIONGRIDRELATEDCONDITIONDIMENSION_H
