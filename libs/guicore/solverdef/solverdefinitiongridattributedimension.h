#ifndef SOLVERDEFINITIONGRIDATTRIBUTEDIMENSION_H
#define SOLVERDEFINITIONGRIDATTRIBUTEDIMENSION_H

#include "../guicore_global.h"
#include "solverdefinitionnode.h"

#include <string>

class QDomElement;
class SolverDefinitionGridAttribute;
class GridAttributeDimensionContainer;
class GridAttributeDimensionSelectWidget;

class GUICOREDLL_EXPORT SolverDefinitionGridAttributeDimension : public SolverDefinitionNode
{
public:
	SolverDefinitionGridAttributeDimension(const QDomElement& node, SolverDefinition* solverDef, SolverDefinitionGridAttribute* att);
	virtual ~SolverDefinitionGridAttributeDimension();

	const std::string& name() const;
	const QString& caption() const;
	const std::string& englishCaption() const;

	SolverDefinitionGridAttribute* attribute() const;

	virtual GridAttributeDimensionContainer* buildContainer() = 0;
	GridAttributeDimensionSelectWidget* buildSelectWidget(GridAttributeDimensionContainer* container);

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

#ifdef _DEBUG
	#include "private/solverdefinitiongridattributedimension_impl.h"
#endif // _DEBUG

#endif // SOLVERDEFINITIONGRIDATTRIBUTEDIMENSION_H
