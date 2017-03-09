#ifndef INPUTCONDITIONCONTAINERFUNCTIONAL_IMPL_H
#define INPUTCONDITIONCONTAINERFUNCTIONAL_IMPL_H

#include "../inputconditioncontainerfunctional.h"

class InputConditionContainerFunctional::Impl
{
public:
	Data m_param;
	std::vector<Data> m_values;

	Data m_paramDefault;
	std::vector<Data> m_valuesDefault;
};

#endif // INPUTCONDITIONCONTAINERFUNCTIONAL_IMPL_H
