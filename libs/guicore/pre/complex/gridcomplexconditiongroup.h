#ifndef GRIDCOMPLEXCONDITIONGROUP_H
#define GRIDCOMPLEXCONDITIONGROUP_H

#include "../../guicore_global.h"

class SolverDefinition;
class InputConditionContainerSet;

class QDomElement;
class QWidget;

class GUICOREDLL_EXPORT GridComplexConditionGroup
{
public:
	GridComplexConditionGroup(SolverDefinition* def, const QDomElement& elem);
	~GridComplexConditionGroup();

	void load(const int fn);
	void save(const int fn);

	QWidget* widget() const;

	InputConditionContainerSet* containerSet() const;

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/gridcomplexconditiongroup_impl.h"
#endif // _DEBUG

#endif // GRIDCOMPLEXCONDITIONGROUP_H
