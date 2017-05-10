#ifndef INPUTCONDITIONCONTAINERINTEGER_IMPL_H
#define INPUTCONDITIONCONTAINERINTEGER_IMPL_H

#include "../inputconditioncontainerinteger.h"

class InputConditionContainerInteger::Impl
{
public:
	Impl();

	int m_value;
	int m_default;
};

#endif // INPUTCONDITIONCONTAINERINTEGER_IMPL_H
