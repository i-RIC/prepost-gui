#include "../../../solverdef/solverdefinitiontranslator.h"
#include "../inputconditioncontainerstring.h"
#include "inputconditioncgnsfile.h"
#include "inputconditionwidgetcgnsresult.h"
#include "inputconditionwidgettooltip.h"

#include <misc/xmlsupport.h>

#include <QComboBox>
#include <QDomElement>
#include <QDomNode>
#include <QDomNodeList>
#include <QHBoxLayout>
#include <QStringList>
#include <QVariant>

InputConditionWidgetCgnsResult::InputConditionWidgetCgnsResult(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file, bool noStretch) :
	InputConditionWidget(defnode),
	m_comboBox {new QComboBox {this}},
	m_cgnsFile {file},
	m_translator {&t}
{
	auto layout = new QHBoxLayout(this);
	if (! noStretch) {
		layout->addStretch(1);
	}
	layout->addWidget(m_comboBox, 1);
	layout->setMargin(InputConditionWidget::margin);
	setLayout(layout);

	m_container = cont;
	setValue(cont->value());
	// connect
	connect(m_container, SIGNAL(valueChanged(QString)), this, SLOT(setValue(QString)));
	connect(m_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(informChange(int)));
	connect(file, SIGNAL(changed()), this, SLOT(handleFileChange()));

	handleFileChange();
}

void InputConditionWidgetCgnsResult::addTooltip(const QString& tooltip)
{
	InputConditionWidgetTooltip* tt = new InputConditionWidgetTooltip(tooltip, this);

	QHBoxLayout* l = dynamic_cast<QHBoxLayout*>(layout());
	l->insertWidget(1, tt);
}

void InputConditionWidgetCgnsResult::setValue(const QString& newvalue)
{
	int index = m_comboBox->findText(newvalue);
	if (index == -1) {
		if (cgnsFile()->isEffective()) {
			// select the first item
			m_comboBox->setCurrentIndex(0);
		}
	} else {
		m_comboBox->setCurrentIndex(index);
	}
}

void InputConditionWidgetCgnsResult::handleFileChange()
{
	if (m_cgnsFile->isEffective()) {
		updateEnums();
	} else {
		m_comboBox->blockSignals(true);
		m_comboBox->clear();
		m_comboBox->addItem(tr("(CGNS file is not selected)"));
		m_comboBox->setCurrentIndex(0);
		m_comboBox->blockSignals(false);
	}
}

void InputConditionWidgetCgnsResult::informChange(int index)
{
	m_container->setValue(m_comboBox->itemText(index));
}

InputConditionCgnsFile* InputConditionWidgetCgnsResult::cgnsFile() const
{
	return m_cgnsFile;
}

void InputConditionWidgetCgnsResult::updateEnums()
{
	m_comboBox->blockSignals(true);
	m_comboBox->clear();
	QStringList enums = getEnums();
	if (enums.size() == 0) {
		m_comboBox->addItem(tr("(There is no result)"));
		m_comboBox->setCurrentIndex(0);
		m_container->setValue("");
	} else {
		for (int i = 0; i < enums.size(); ++i) {
			const QString& e = enums.at(i);
			m_comboBox->addItem(e);
		}
		auto v = m_container->value();
		if (v.isEmpty()) {
			if (enums.size() > 0) {
				auto newValue = enums.at(0);
				setValue(newValue);
				m_container->setValue(newValue);
			}
		} else {
			setValue(v);
		}
	}
	m_comboBox->blockSignals(false);
}
