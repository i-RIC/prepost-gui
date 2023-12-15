#ifndef POSTSTRINGRESULTENGINE_IMPL_H
#define POSTSTRINGRESULTENGINE_IMPL_H

#include "../poststringresultengine.h"
#include "poststringresultengine_argument.h"

#include <QJSEngine>
#include <QObject>

class PostStringResultEngine::Impl
{
public:
	Impl();

	QJSValue m_function;
	std::vector<Argument> m_arguments;

	QJSEngine m_jsEngine;
};

#endif // POSTSTRINGRESULTENGINE_IMPL_H
