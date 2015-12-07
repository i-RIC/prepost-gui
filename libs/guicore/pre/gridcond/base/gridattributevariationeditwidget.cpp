#include "gridattributevariationeditwidget.h"

GridAttributeVariationEditWidget::GridAttributeVariationEditWidget(QWidget* parent, SolverDefinitionGridAttribute* condition) :
	QWidget {parent},
	m_gridAttribute {condition}
{}

GridAttributeVariationEditWidget::~GridAttributeVariationEditWidget()
{}

void GridAttributeVariationEditWidget::clearValue()
{
	setupWidget();
}

SolverDefinitionGridAttribute* GridAttributeVariationEditWidget::gridAttribute() const
{
	return m_gridAttribute;
}
