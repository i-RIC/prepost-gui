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
};

#endif // INPUTCONDITIONCONTAINER_IMPL_H
