#include "../../../solverdef/solverdefinitiontranslator.h"
#include "../inputconditioncontainerinteger.h"
#include "inputconditionwidgetinteger.h"
#include "inputconditionwidgettooltip.h"

#include <guibase/widget/integernumbereditwidget.h>

#include <QDomElement>
#include <QHBoxLayout>

InputConditionWidgetInteger::InputConditionWidgetInteger(QDomNode defnode, const SolverDefinitionTranslator& /*t*/, InputConditionContainerInteger* cont, const InputConditionContainerSet& /*cs*/, bool noStretch) :
	InputConditionWidget(defnode)
{
	m_spinBox = new IntegerNumberEditWidget(this);
	m_spinBox->setMinimumWidth(100);
	m_spinBox->setAlignment(Qt::AlignRight);

	QHBoxLayout* layout = new QHBoxLayout(this);
	if (! noStretch) {
		layout->addStretch(1);
	}
	layout->addWidget(m_spinBox, 1);
	layout->setMargin(InputConditionWidget::margin);
	setLayout(layout);

	QDomElement defElem = defnode.toElement();
	// min
	QString minstr = defElem.attribute("min");
	if (minstr != "") {
		bool ok;
		int minval = minstr.toInt(&ok);
		if (ok) {
			m_spinBox->setMinimum(minval);
		}
	}
	// max
	QString maxstr = defElem.attribute("max");
	if (maxstr != "") {
		bool ok;
		int maxval = maxstr.toInt(&ok);
		if (ok) {
			m_spinBox->setMaximum(maxval);
		}
	}
	m_container = cont;
	m_spinBox->setValue(m_container->value());
	// connect
	connect(m_container, SIGNAL(valueChanged(int)), m_spinBox, SLOT(setValue(int)));
	connect(m_spinBox, SIGNAL(valueChanged(int)), this, SLOT(informChange(int)));
}

void InputConditionWidgetInteger::addTooltip(const QString& tooltip)
{
	InputConditionWidgetTooltip* tt = new InputConditionWidgetTooltip(tooltip, this);

	QHBoxLayout* l = dynamic_cast<QHBoxLayout*>(layout());
	l->insertWidget(1, tt);
}

void InputConditionWidgetInteger::informChange(int value)
{
	m_container->setValue(value);
}
