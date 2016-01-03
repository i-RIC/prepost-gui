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
	m_valueCleared = true;
	m_valueSelected = false;
	setupWidget();
}

bool GridAttributeEditWidget::valueSelected() const
{
	getValueFromInnerWidget();
	return m_valueSelected;
}
