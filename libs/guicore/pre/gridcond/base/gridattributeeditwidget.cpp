#include "gridattributeeditwidget.h"

GridAttributeEditWidget::GridAttributeEditWidget(QWidget* parent, SolverDefinitionGridAttribute* att) :
	QWidget(parent),
	m_gridAttribute {att}
{}

SolverDefinitionGridAttribute* GridAttributeEditWidget::gridAttribute() const
{
	return m_gridAttribute;
}

void GridAttributeEditWidget::clearValue()
{
	m_isValueCleared = true;
	m_isValueSelected = false;
	setupWidget();
}

bool GridAttributeEditWidget::isValueSelected() const
{
	getValueFromInnerWidget();
	return m_isValueSelected;
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
	return m_isValueCleared;
}

void GridAttributeEditWidget::setValueCleared(bool cleared)
{
	m_isValueCleared = cleared;
}

void GridAttributeEditWidget::setValueSelected(bool selected) const
{
	m_isValueSelected = selected;
}
