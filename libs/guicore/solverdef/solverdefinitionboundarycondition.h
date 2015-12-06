#ifndef SOLVERDEFINITIONBOUNDARYCONDITION_H
#define SOLVERDEFINITIONBOUNDARYCONDITION_H

#include "../guicore_global.h"
#include "solverdefinitionnode.h"

#include <string>

class QDomElement;
class QString;

class GUICOREDLL_EXPORT SolverDefinitionBoundaryCondition : public SolverDefinitionNode
{
public:
	enum Position {pNode, pCell, pEdge};
	SolverDefinitionBoundaryCondition(QDomElement node, const SolverDefinitionTranslator& translator);
	~SolverDefinitionBoundaryCondition();

	const std::string& name() const;
	const QString& caption() const;
	const std::string& englishCaption() const;
	Position position() const;

	void setCaption(const QString& caption);

	const QDomElement& element() const;

private:
	class Impl;
	Impl* m_impl;
};

#endif // SOLVERDEFINITIONBOUNDARYCONDITION_H
