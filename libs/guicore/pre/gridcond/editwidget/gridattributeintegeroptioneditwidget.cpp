#include "gridattributeintegeroptioneditwidget.h"

#include <QComboBox>
#include <QVBoxLayout>

GridAttributeIntegerOptionEditWidget::GridAttributeIntegerOptionEditWidget(QWidget* parent, SolverDefinitionGridAttributeT<int>* cond) :
	GridAttributeEditWidgetT<int>(parent, cond)
{
	m_widget = new QComboBox(this);
	m_widget->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	QVBoxLayout* l = new QVBoxLayout();
	l->setMargin(0);
	l->addWidget(m_widget);
	setLayout(l);
	setSizePolicy(m_widget->sizePolicy());
}

GridAttributeIntegerOptionEditWidget::~GridAttributeIntegerOptionEditWidget()
{}

const QMap<int, QString>& GridAttributeIntegerOptionEditWidget::enumerations() const
{
	return m_enumerations;
}

void GridAttributeIntegerOptionEditWidget::setEnumerations(const QMap<int, QString>& enums)
{
	m_enumerations = enums;
}

void GridAttributeIntegerOptionEditWidget::setupWidget()
{
	m_widget->clear();
	m_values.clear();

	int i = 0;
	if (isValueCleared()) {
		m_widget->addItem("");
		m_values.append(0);
		++i;
	}
	int index = 0;
	for (auto it = m_enumerations.begin(); it != m_enumerations.end(); ++it) {
		m_widget->addItem(it.value());
		m_values.append(it.key());
		if (! isValueCleared() && it.key() == m_value) {index = i;}
		++i;
	}
	m_widget->setCurrentIndex(index);
}

void GridAttributeIntegerOptionEditWidget::getValueFromInnerWidget() const
{
	int index = m_widget->currentIndex();
	setValueSelected(! (isValueCleared() && index == 0));
	m_value = m_values.at(index);
}

QWidget* GridAttributeIntegerOptionEditWidget::editWidget() const
{
	return m_widget;
}
