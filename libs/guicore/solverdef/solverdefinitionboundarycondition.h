#ifndef SOLVERDEFINITIONBOUNDARYCONDITION_H
#define SOLVERDEFINITIONBOUNDARYCONDITION_H

#include "../guicore_global.h"
#include "solverdefinitionnode.h"

class QDomElement;
class QString;

class GUICOREDLL_EXPORT SolverDefinitionBoundaryCondition : public SolverDefinitionNode
{
public:
	enum Position {pNode, pCell, pEdge};
	/// Constructor
	SolverDefinitionBoundaryCondition(QDomElement node, const SolverDefinitionTranslator& translator);
	/// Destructor
	~SolverDefinitionBoundaryCondition();
	const QString& name() const;
	const QString& caption() const;
	const QString& englishCaption() const;
	void setCaption(const QString& caption);
	const QDomElement& element() const;
	Position position() const;

private:
	class Impl;
	Impl* m_impl;
};

#endif // SOLVERDEFINITIONBOUNDARYCONDITION_H
