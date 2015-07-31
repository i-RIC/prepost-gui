#ifndef SOLVERDEFINITIONGRIDRELATEDCONDITIONDIMENSION_H
#define SOLVERDEFINITIONGRIDRELATEDCONDITIONDIMENSION_H

#include "../guicore_global.h"
#include "solverdefinitionnode.h"

class QDomElement;
class SolverDefinitionGridAttribute;
class GridAttributeDimensionContainer;
class GridAttributeDimensionSelectWidget;

class GUICOREDLL_EXPORT SolverDefinitionGridAttributeDimension : public SolverDefinitionNode
{

public:
	SolverDefinitionGridAttributeDimension(const QDomElement& node, const SolverDefinitionTranslator& translator, SolverDefinitionGridAttribute* att);
	virtual ~SolverDefinitionGridAttributeDimension();
	const QString& name() const;
	const QString& caption() const;
	const QString& englishCaption() const;
	SolverDefinitionGridAttribute* attribute() const;
	virtual GridAttributeDimensionContainer* buildContainer() = 0;
	GridAttributeDimensionSelectWidget* buildSelectWidget(GridAttributeDimensionContainer* container);

private:
	class Impl;
	Impl* m_impl;
};

#endif // SOLVERDEFINITIONGRIDRELATEDCONDITIONDIMENSION_H
