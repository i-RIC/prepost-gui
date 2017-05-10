#ifndef INPUTCONDITIONCONTAINER_IMPL_H
#define INPUTCONDITIONCONTAINER_IMPL_H

#include "../inputconditioncontainer.h"

#include <QString>

class InputConditionContainer::Impl
{
public:
	Impl (const std::string& name, const QString& caption);

	std::string m_name;
	QString m_caption;

	bool m_isBoundaryCondition;
	std::string m_bcName;
	int m_bcIndex;

	bool m_isComplexCondition;
	std::string m_complexName;
	int m_complexIndex;
};

#endif // INPUTCONDITIONCONTAINER_IMPL_H
