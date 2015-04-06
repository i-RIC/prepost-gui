#include "../../../solverdef/solverdefinitiongridrelatedcomplexcondition.h"
#include "gridrelatedcomplexconditioneditwidget.h"

#include <QComboBox>
#include <QVBoxLayout>

GridRelatedComplexConditionEditWidget::GridRelatedComplexConditionEditWidget(QWidget* parent, SolverDefinitionGridRelatedComplexCondition* cond)
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

GridRelatedComplexConditionEditWidget::~GridRelatedComplexConditionEditWidget()
{}

void GridRelatedComplexConditionEditWidget::setupWidget()
{
	m_widget->clear();
	m_values.clear();

	QMap<int, QString>::iterator it;
	int i = 0;
	int index = -1;
	int defaultIndex = 0;
	if (m_valueCleared){
		m_widget->addItem("");
		m_values.append(0);
		index = 0;
		++i;
	}
	for (it = m_enumerations.begin(); it != m_enumerations.end(); ++it){
		m_widget->addItem(it.value());
		m_values.append(it.key());
		if (! m_valueCleared && it.key() == m_value){index = i;}
		if (it.key() == m_defaultIndex){defaultIndex = i;}
		++i;
	}
	if (index == -1){
		index = defaultIndex;
	}
	m_widget->setCurrentIndex(index);
}

void GridRelatedComplexConditionEditWidget::getValueFromInnerWidget()
{
	int index = m_widget->currentIndex();
	m_valueSelected = ! (m_valueCleared && index == 0);
	m_value = m_values.at(index);
}

QSize GridRelatedComplexConditionEditWidget::sizeHint() const
{
	return m_widget->sizeHint();
}
QSize GridRelatedComplexConditionEditWidget::minimumSizeHint() const
{
	return m_widget->sizeHint();
}

