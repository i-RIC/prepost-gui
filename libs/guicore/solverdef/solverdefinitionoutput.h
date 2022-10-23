#ifndef SOLVERDEFINITIONOUTPUT_H
#define SOLVERDEFINITIONOUTPUT_H

#include "../guicore_global.h"
#include "solverdefinitionnode.h"

class SolverDefinitionTranslator;

class GUICOREDLL_EXPORT SolverDefinitionOutput : public SolverDefinitionNode
{
public:
	SolverDefinitionOutput(const std::string name);
	SolverDefinitionOutput(QDomElement node, SolverDefinition* dolverDef, const SolverDefinitionTranslator& translator);
	virtual ~SolverDefinitionOutput();

	const std::string& name() const;
	const QString& caption() const;

	const std::string& englishCaption() const;

private:
	class Impl;
	Impl* impl;
};

#endif // SOLVERDEFINITIONOUTPUT_H
