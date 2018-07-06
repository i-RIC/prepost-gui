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

	// wml2 data
	QString m_wml2URL;
	Convert m_paramConvert;
	std::vector<Convert> m_valuesConvert;
	QString m_siteID;
	QString m_startDate;
	QString m_endDate;
};

#endif // INPUTCONDITIONCONTAINERFUNCTIONAL_IMPL_H
