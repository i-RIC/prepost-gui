#include "../../../solverdef/solverdefinitiontranslator.h"
#include "../inputconditioncontainerinteger.h"
#include "inputconditiondependencychecksubenumerations.h"
#include "inputconditionwidgetintegeroption.h"
#include "inputconditionwidgettooltip.h"

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
		// store default list
		m_enumerations.push_back(std::pair<QString, QVariant>(caption, QVariant(nomValue)));
		m_comboBox->addItem(caption, QVariant(nomValue));
	}
	m_container = cont;
	setValue(cont->value());

	if (hasSubEnums(defnode)) {
		// SubEnumerations must have an Enumerations parent
		QDomNode enumsNode = iRIC::getChildNode(defnode, "Enumerations");
		QDomNodeList children = enumsNode.childNodes();
		for (int i = 0; i < children.count(); ++i) {
			QDomNode subEnums = children.at(i);
			if (subEnums.nodeName() != "SubEnumerations") { continue; }
			// create unique name to lookup list
			QString name = subEnumerationsName(subEnums, m_subEnumerations.size());
			QDomNodeList nodes = subEnums.childNodes();
			for (int n = 0; n < nodes.size(); ++n) {
				QDomNode enumNode = nodes.at(n);
				if (enumNode.nodeName() != "Enumeration") { continue; }
				QDomElement enumElem = enumNode.toElement();
				int value = enumElem.attribute("value").toInt();
				QString caption = t.translate(enumElem.attribute("caption"));
				// append to sub-list
				m_subEnumerations[name].push_back({ caption, QVariant(value) });
			}
		}
	}
	// check
	m_checkSubEnumerations = new InputConditionDependencyCheckSubEnumerations(this);

	// connect
	connect(m_container, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
	connect(m_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(informChange(int)));
}

void InputConditionWidgetIntegerOption::addTooltip(const QString& tooltip)
{
	InputConditionWidgetTooltip* tt = new InputConditionWidgetTooltip(tooltip, this);

	QHBoxLayout* l = dynamic_cast<QHBoxLayout*>(layout());
	l->insertWidget(1, tt);
}

void InputConditionWidgetIntegerOption::setDisabled(bool disable)
{
	m_comboBox->setDisabled(disable);
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

QString InputConditionWidgetIntegerOption::subEnumerationsName(const QDomNode& subEnumerationsNode, size_t index)
{
	Q_UNUSED(subEnumerationsNode);
	return QString("SubEnums-%1").arg(index);
}

void InputConditionWidgetIntegerOption::activateSubEnumerations(const QString& name)
{
	std::map< QString, std::list< std::pair<QString, QVariant> > >::iterator it = m_subEnumerations.find(name);
	if (it == m_subEnumerations.end()) { return; }

	m_comboBox->clear();
	for (auto& p : it->second) {
		m_comboBox->addItem(p.first, p.second);
	}
}

void InputConditionWidgetIntegerOption::inactivateSubEnumerations()
{
	m_comboBox->clear();
	for (auto& p : m_enumerations) {
		m_comboBox->addItem(p.first, p.second);
	}
}

InputConditionDependencyCheckSubEnumerations* InputConditionWidgetIntegerOption::checkSubEnumerations() const
{
	return m_checkSubEnumerations;
}
