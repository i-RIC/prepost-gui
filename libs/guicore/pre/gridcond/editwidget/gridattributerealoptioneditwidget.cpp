#include "gridattributerealoptioneditwidget.h"

#include <QComboBox>
#include <QVBoxLayout>

GridAttributeRealOptionEditWidget::GridAttributeRealOptionEditWidget(QWidget* parent, SolverDefinitionGridAttributeT<double>* cond)
	: GridAttributeEditWidgetT<double>(parent, cond)
{
	m_widget = new QComboBox(this);
	m_widget->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	QVBoxLayout* l = new QVBoxLayout();
	l->setMargin(0);
	l->addWidget(m_widget);
	setLayout(l);
	setSizePolicy(m_widget->sizePolicy());
}

GridAttributeRealOptionEditWidget::~GridAttributeRealOptionEditWidget()
{}

void GridAttributeRealOptionEditWidget::setupWidget()
{
	m_widget->clear();
	m_values.clear();

	int i = 0;
	if (m_valueCleared) {
		m_widget->addItem("");
		m_values.append(0);
		++i;
	}
	int index = 0;
	for (auto it = m_enumerations.begin(); it != m_enumerations.end(); ++it) {
		m_widget->addItem(it.value());
		m_values.append(it.key());
		if (it.key() == m_value) {index = i;}
		++i;
	}
	m_widget->setCurrentIndex(index);
}

void GridAttributeRealOptionEditWidget::getValueFromInnerWidget()
{
	int index = m_widget->currentIndex();
	m_valueSelected = !(m_valueCleared && index == 1);
	m_value = m_values.at(index);
}

QSize GridAttributeRealOptionEditWidget::sizeHint() const
{
	return m_widget->sizeHint();
}

QSize GridAttributeRealOptionEditWidget::minimumSizeHint() const
{
	return m_widget->sizeHint();
}

