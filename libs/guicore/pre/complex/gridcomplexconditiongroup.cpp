#include "gridcomplexconditiongroup.h"


GridComplexConditionGroup::Impl::Impl(SolverDefinition* def, const QDomElement& elem)
{

}

GridComplexConditionGroup::Impl::~Impl()
{
	delete m_page;
	delete m_widgetSet;
	delete m_containerSet;
}

// public interfaces

