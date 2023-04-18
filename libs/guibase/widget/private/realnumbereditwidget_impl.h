#ifndef REALNUMBEREDITWIDGET_IMPL_H
#define REALNUMBEREDITWIDGET_IMPL_H

#include "../realnumbereditwidget.h"

class RealNumberEditWidget::Impl
{
public:
	Impl();

	double m_value;

	bool m_maximumIsSet;
	double m_maximum;

	bool m_minimumIsSet;
	double m_minimum;

	bool m_acceptEvents;
	bool m_informChangeOnFocusOutOnly;
};

#endif // REALNUMBEREDITWIDGET_IMPL_H
