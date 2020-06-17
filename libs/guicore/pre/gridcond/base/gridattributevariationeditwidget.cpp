#include "gridattributevariationeditwidget.h"

GridAttributeVariationEditWidget::GridAttributeVariationEditWidget(QWidget* parent, SolverDefinitionGridAttribute* condition) :
	QWidget {parent},
	m_gridAttribute {condition},
	m_mode {Difference}
{}

GridAttributeVariationEditWidget::~GridAttributeVariationEditWidget()
{}

SolverDefinitionGridAttribute* GridAttributeVariationEditWidget::gridAttribute() const
{
	return m_gridAttribute;
}

void GridAttributeVariationEditWidget::setMode(Mode mode)
{
	m_mode = mode;
}
