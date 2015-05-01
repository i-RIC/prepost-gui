#include "cgnsfileinputconditionwidgetintegeroption.h"
#include "cgnsfileinputconditioncontainerinteger.h"
#include "../../solverdef/solverdefinitiontranslator.h"
#include <misc/xmlsupport.h>

#include <QDomNode>
#include <QDomNodeList>
#include <QDomElement>
#include <QVariant>
#include <QComboBox>
#include <QHBoxLayout>

CgnsFileInputConditionWidgetIntegerOption::CgnsFileInputConditionWidgetIntegerOption(QDomNode defnode, const SolverDefinitionTranslator& t, CgnsFileInputConditionContainerInteger* cont) : CgnsFileInputConditionWidget(defnode)
{
	// add nominations;
	QList<QDomNode> noms = getEnums(defnode);
	m_comboBox = new QComboBox(this);
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addStretch(1);
	layout->addWidget(m_comboBox, 1);
	layout->setMargin(CgnsFileInputConditionWidget::margin);
	setLayout(layout);

	for (unsigned int i = 0; i < noms.length(); ++i) {
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

void CgnsFileInputConditionWidgetIntegerOption::setValue(int newvalue)
{
	int index = m_comboBox->findData(QVariant(newvalue));
	m_comboBox->setCurrentIndex(index);
}

void CgnsFileInputConditionWidgetIntegerOption::informChange(int index)
{
	int value = m_comboBox->itemData(index).toInt();
	m_container->setValue(value);
}
