#ifndef POSTSTRINGRESULT_IMPL_H
#define POSTSTRINGRESULT_IMPL_H

#include "../poststringresult.h"

class PostStringResult::Impl
{
public:
	Impl();
	~Impl();

	std::vector<PostStringResultArgument*> m_arguments;
	QString m_script;

	QString m_value;
	QJSValue m_function;
	QJSEngine m_jsEngine;
};

#endif // POSTSTRINGRESULT_IMPL_H
