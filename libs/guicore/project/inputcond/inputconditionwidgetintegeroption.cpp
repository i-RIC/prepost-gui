#include "../../solverdef/solverdefinitiontranslator.h"
#include "inputconditioncontainerinteger.h"
#include "inputconditionwidgetintegeroption.h"

#include <misc/xmlsupport.h>

#include <QComboBox>
#include <QDomElement>
#include <QDomNode>
#include <QDomNodeList>
#include <QHBoxLayout>
#include <QVariant>

InputConditionWidgetIntegerOption::InputConditionWidgetIntegerOption(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerInteger* cont) : InputConditionWidget(defnode)
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
		int nomValue = nomElem.attribute("value").toInt();
		QString caption = t.translate(nomElem.attribute("caption"));
		m_comboBox->addItem(caption, QVariant(nomValue));
	}
	m_container = cont;
	setValue(cont->value());
	// connect
	connect(m_container, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
	connect(m_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(informChange(int)));
}

void InputConditionWidgetIntegerOption::setValue(int newvalue)
{
	int index = m_comboBox->findData(QVariant(newvalue));
	m_comboBox->setCurrentIndex(index);
}

void InputConditionWidgetIntegerOption::informChange(int index)
{
	int value = m_comboBox->itemData(index).toInt();
	m_container->setValue(value);
}
