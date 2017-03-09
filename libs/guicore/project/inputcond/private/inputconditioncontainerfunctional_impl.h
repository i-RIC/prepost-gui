#ifndef INPUTCONDITIONCONTAINERFUNCTIONAL_IMPL_H
#define INPUTCONDITIONCONTAINERFUNCTIONAL_IMPL_H

#include "../inputconditioncontainerfunctional.h"

class InputConditionContainerFunctional::Impl
{
public:
	Data m_param;
	QList<Data> m_values;

	Data m_paramDefault;
	QList<Data> m_valuesDefault;
};

#endif // INPUTCONDITIONCONTAINERFUNCTIONAL_IMPL_H
