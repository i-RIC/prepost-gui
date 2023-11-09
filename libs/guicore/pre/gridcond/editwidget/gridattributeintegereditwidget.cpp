#include "gridattributeintegereditwidget.h"

#include <guibase/widget/integernumbereditwidget.h>

#include <QVBoxLayout>

GridAttributeIntegerEditWidget::GridAttributeIntegerEditWidget(QWidget* parent, SolverDefinitionGridAttributeT<int>* cond) :
	GridAttributeEditWidgetT<int, vtkIntArray>(parent, cond)
{
	m_widget = new IntegerNumberEditWidget(this);
	QVBoxLayout* l = new QVBoxLayout();
	l->setMargin(0);
	l->addWidget(m_widget);
	setLayout(l);
	if (cond->variantMaximumValue() != "") {
		bool ok;
		int max = cond->variantMaximumValue().toInt(&ok);
		if (ok) {
			m_widget->setMaximum(max);
		}
	}
	if (cond->variantMinimumValue() != "") {
		bool ok;
		int min = cond->variantMinimumValue().toInt(&ok);
		if (ok) {
			m_widget->setMinimum(min);
		}
	}
	setSizePolicy(m_widget->sizePolicy());
}

GridAttributeIntegerEditWidget::~GridAttributeIntegerEditWidget()
{}

void GridAttributeIntegerEditWidget::getValueFromInnerWidget() const
{
	if (m_widget->cleanText() == "") {
		return;
	}
	setValueCleared(false);
	setValueSelected(true);

	m_value = m_widget->value();
}

QWidget* GridAttributeIntegerEditWidget::editWidget() const
{
	return m_widget;
}

void GridAttributeIntegerEditWidget::setupWidget()
{
	if (isValueCleared()) {
		m_widget->clear();
	} else {
		m_widget->setValue(m_value);
	}
}
