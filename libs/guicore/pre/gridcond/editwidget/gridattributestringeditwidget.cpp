#include "gridattributestringeditwidget.h"

#include <QLineEdit>
#include <QVBoxLayout>

GridAttributeStringEditWidget::GridAttributeStringEditWidget(QWidget* parent, SolverDefinitionGridAttributeT<std::string>* cond) :
	GridAttributeEditWidgetT<std::string, vtkStringArray>(parent, cond)
{
	m_widget = new QLineEdit(this);
	QVBoxLayout* l = new QVBoxLayout();
	l->setMargin(0);
	l->addWidget(m_widget);
	setLayout(l);

	setSizePolicy(m_widget->sizePolicy());
}

GridAttributeStringEditWidget::~GridAttributeStringEditWidget()
{}

void GridAttributeStringEditWidget::setupWidget()
{
	if (isValueCleared()) {
		m_widget->clear();
	} else {
		m_widget->setText(m_value.c_str());
	}
}
void GridAttributeStringEditWidget::getValueFromInnerWidget() const
{
	if (m_widget->text() == "") {
		return;
	}
	setValueCleared(false);
	setValueSelected(true);

	m_value = iRIC::toStr(m_widget->text());
}

QWidget* GridAttributeStringEditWidget::editWidget() const
{
	return m_widget;
}
