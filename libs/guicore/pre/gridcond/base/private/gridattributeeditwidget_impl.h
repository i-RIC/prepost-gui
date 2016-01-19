#ifndef GRIDATTRIBUTEEDITWIDGET_IMPL_H
#define GRIDATTRIBUTEEDITWIDGET_IMPL_H

#include "../gridattributeeditwidget.h"

class GridAttributeEditWidget::Impl
{
public:
	Impl(SolverDefinitionGridAttribute* att);

	mutable bool m_isValueSelected;
	mutable bool m_isValueCleared;

	SolverDefinitionGridAttribute* m_gridAttribute;
};

#endif // GRIDATTRIBUTEEDITWIDGET_IMPL_H

