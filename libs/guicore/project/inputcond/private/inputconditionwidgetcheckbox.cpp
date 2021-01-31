#include "../../../solverdef/solverdefinitiontranslator.h"
#include "../inputconditioncontainerinteger.h"
#include "inputconditionwidgetcheckbox.h"
#include "inputconditionwidgettooltip.h"

#include <QCheckBox>
#include <QHBoxLayout>

InputConditionWidgetCheckbox::InputConditionWidgetCheckbox(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerInteger* cont) :
	InputConditionWidget(defnode),
	m_checkBox {new QCheckBox(this)}
{
	auto layout = new QHBoxLayout(this);
	layout->addWidget(m_checkBox);
	layout->addStretch(1);
	layout->setMargin(InputConditionWidget::margin);
	setLayout(layout);

	auto defElem = defnode.toElement();
	QString caption = t.translate(defElem.attribute("caption", InputConditionWidgetCheckbox::tr("Enabled")));
	m_checkBox->setText(caption);
	m_container = cont;
	setValue(cont->value());

	connect(m_container, SIGNAL(valueChanged(int)), this, SLOT(setValue(int)));
	connect(m_checkBox, SIGNAL(toggled(bool)), this, SLOT(informChange(bool)));
}

void InputConditionWidgetCheckbox::addTooltip(const QString& tooltip)
{
	InputConditionWidgetTooltip* tt = new InputConditionWidgetTooltip(tooltip, this);

	QHBoxLayout* l = dynamic_cast<QHBoxLayout*>(layout());
	l->insertWidget(1, tt);
}

void InputConditionWidgetCheckbox::setDisabled(bool disable)
{
	m_checkBox->setDisabled(disable);
}

void InputConditionWidgetCheckbox::setValue(int newvalue)
{
	bool checked = (newvalue != 0);
	m_checkBox->setChecked(checked);
}

void InputConditionWidgetCheckbox::informChange(bool checked)
{
	int value = 0;
	if (checked) {value = 1;}

	m_container->setValue(value);
}
