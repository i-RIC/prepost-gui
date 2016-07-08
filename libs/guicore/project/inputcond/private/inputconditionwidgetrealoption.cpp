#include "../../../solverdef/solverdefinitiontranslator.h"
#include "../inputconditioncontainerreal.h"
#include "inputconditionwidgetrealoption.h"
#include "inputconditionwidgettooltip.h"

#include <misc/xmlsupport.h>

#include <QComboBox>
#include <QDomElement>
#include <QDomNode>
#include <QDomNodeList>
#include <QHBoxLayout>
#include <QVariant>

InputConditionWidgetRealOption::InputConditionWidgetRealOption(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerReal* cont) : InputConditionWidget(defnode)
{
	// add nominations;
	QList<QDomNode> noms = getEnums(defnode);
	m_comboBox = new QComboBox(this);
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addStretch(1);
	layout->addWidget(m_comboBox, 1);
	layout->setMargin(InputConditionWidget::margin);
	setLayout(layout);

	for (int i = 0; i < noms.length(); ++i) {
		QDomNode nomNode = noms.at(i);
		QDomElement nomElem = nomNode.toElement();
		double nomValue = nomElem.attribute("value").toDouble();
		QString caption = t.translate(nomElem.attribute("caption"));
		m_comboBox->addItem(caption, QVariant(nomValue));
	}
	m_container = cont;
	setValue(cont->value());
	// connect
	connect(m_container, SIGNAL(valueChanged(double)), m_comboBox, SLOT(setValue(double)));
	connect(m_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(informChange(int)));
}

void InputConditionWidgetRealOption::addTooltip(const QString& tooltip)
{
	InputConditionWidgetTooltip* tt = new InputConditionWidgetTooltip(tooltip, this);

	QHBoxLayout* l = dynamic_cast<QHBoxLayout*>(layout());
	l->insertWidget(1, tt);
}

void InputConditionWidgetRealOption::setDisabled(bool disable)
{
	m_comboBox->setDisabled(disable);
}

void InputConditionWidgetRealOption::setValue(double newvalue)
{
	int index = m_comboBox->findData(QVariant(newvalue));
	m_comboBox->setCurrentIndex(index);
}

void InputConditionWidgetRealOption::informChange(int index)
{
	double value = m_comboBox->itemData(index).toDouble();
	m_container->setValue(value);
}
