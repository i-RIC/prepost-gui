#ifndef INPUTCONDITIONCONTAINERREAL_IMPL_H
#define INPUTCONDITIONCONTAINERREAL_IMPL_H

#include "../inputconditioncontainerreal.h"

class InputConditionContainerReal::Impl
{
public:
	Impl();

	double m_value;
	double m_default;
};

#endif // INPUTCONDITIONCONTAINERREAL_IMPL_H
