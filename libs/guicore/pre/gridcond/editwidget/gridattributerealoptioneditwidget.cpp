#include "gridattributerealoptioneditwidget.h"

#include <QComboBox>
#include <QVBoxLayout>

GridAttributeRealOptionEditWidget::GridAttributeRealOptionEditWidget(QWidget* parent, SolverDefinitionGridAttributeT<double>* cond) :
	GridAttributeEditWidgetT<double, vtkDoubleArray>(parent, cond)
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

const QMap<double, QString>& GridAttributeRealOptionEditWidget::enumerations() const
{
	return m_enumerations;
}

void GridAttributeRealOptionEditWidget::setEnumerations(const QMap<double, QString>& enums)
{
	m_enumerations = enums;
}

void GridAttributeRealOptionEditWidget::setupWidget()
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
		if (it.key() == m_value) {index = i;}
		++i;
	}
	m_widget->setCurrentIndex(index);
}

void GridAttributeRealOptionEditWidget::getValueFromInnerWidget() const
{
	int index = m_widget->currentIndex();
	setValueSelected(! (isValueCleared() && index == 1));
	m_value = m_values.at(index);
}

QWidget* GridAttributeRealOptionEditWidget::editWidget() const
{
	return m_widget;
}
