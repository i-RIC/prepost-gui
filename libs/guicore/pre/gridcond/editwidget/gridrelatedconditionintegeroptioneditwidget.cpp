#include "gridrelatedconditionintegeroptioneditwidget.h"

#include <QComboBox>
#include <QVBoxLayout>

GridRelatedConditionIntegerOptionEditWidget::GridRelatedConditionIntegerOptionEditWidget(QWidget* parent, SolverDefinitionGridRelatedConditionT<int>* cond)
	: GridRelatedConditionEditWidgetT<int>(parent, cond)
{
	m_widget = new QComboBox(this);
	m_widget->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	QVBoxLayout* l = new QVBoxLayout();
	l->setMargin(0);
	l->addWidget(m_widget);
	setLayout(l);
	setSizePolicy(m_widget->sizePolicy());
}

GridRelatedConditionIntegerOptionEditWidget::~GridRelatedConditionIntegerOptionEditWidget()
{}

void GridRelatedConditionIntegerOptionEditWidget::setupWidget()
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
		if (! m_valueCleared && it.key() == m_value) {index = i;}
		++i;
	}
	m_widget->setCurrentIndex(index);
}

void GridRelatedConditionIntegerOptionEditWidget::getValueFromInnerWidget()
{
	int index = m_widget->currentIndex();
	m_valueSelected = !(m_valueCleared && index == 0);
	m_value = m_values.at(index);
}

QSize GridRelatedConditionIntegerOptionEditWidget::sizeHint() const
{
	return m_widget->sizeHint();
}
QSize GridRelatedConditionIntegerOptionEditWidget::minimumSizeHint() const
{
	return m_widget->sizeHint();
}

