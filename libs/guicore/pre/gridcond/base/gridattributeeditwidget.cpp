#include "gridattributeeditwidget.h"
#include "private/gridattributeeditwidget_impl.h"

GridAttributeEditWidget::Impl::Impl(SolverDefinitionGridAttribute *att) :
	m_isValueSelected {false},
	m_isValueCleared {false},
	m_gridAttribute {att}
{}

GridAttributeEditWidget::GridAttributeEditWidget(QWidget* parent, SolverDefinitionGridAttribute* att) :
	QWidget {parent},
	impl {new Impl {att}}
{}

GridAttributeEditWidget::~GridAttributeEditWidget()
{}

SolverDefinitionGridAttribute* GridAttributeEditWidget::gridAttribute() const
{
	return impl->m_gridAttribute;
}

void GridAttributeEditWidget::clearValue()
{
	impl->m_isValueCleared = true;
	impl->m_isValueSelected = false;
	setupWidget();
}

bool GridAttributeEditWidget::isValueSelected() const
{
	getValueFromInnerWidget();
	return impl->m_isValueSelected;
}

QSize GridAttributeEditWidget::sizeHint() const
{
	return editWidget()->sizeHint();
}

QSize GridAttributeEditWidget::minimumSizeHint() const
{
	return editWidget()->sizeHint();
}

bool GridAttributeEditWidget::isValueCleared() const
{
	return impl->m_isValueCleared;
}

void GridAttributeEditWidget::setValueCleared(bool cleared) const
{
	impl->m_isValueCleared = cleared;
}

void GridAttributeEditWidget::setValueSelected(bool selected) const
{
	impl->m_isValueSelected = selected;
}
