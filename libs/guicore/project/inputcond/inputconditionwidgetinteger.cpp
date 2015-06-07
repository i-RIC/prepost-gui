#include "../../solverdef/solverdefinitiontranslator.h"
#include "inputconditioncontainerinteger.h"
#include "inputconditioncontainerset.h"
#include "inputconditionwidgetinteger.h"
//#include "../Dependency/ccdMisc.h"

#include <QDomElement>
#include <QHBoxLayout>
#include <QSpinBox>

InputConditionWidgetInteger::InputConditionWidgetInteger(QDomNode defnode, const SolverDefinitionTranslator& /*t*/, InputConditionContainerInteger* cont, const InputConditionContainerSet& /*cs*/) : InputConditionWidget(defnode)
{
	m_spinBox = new QSpinBox(this);
	m_spinBox->setMinimumWidth(100);
	m_spinBox->setAlignment(Qt::AlignRight);
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addStretch(1);
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
		} else {
			// todo build minimum container.
			/*
			   CalculationCondition::Dependency::buildMinimumContainer(
			    atts.namedItem(minstr).nodeValue(), cs, this
			   );
			 */
		}
	}
	// max
	QString maxstr = defElem.attribute("max");
	if (maxstr != "") {
		bool ok;
		int maxval = maxstr.toInt(&ok);
		if (ok) {
			m_spinBox->setMaximum(maxval);
		} else {
			// todo build maximum container.
			/*
			   CalculationCondition::Dependency::buildMaximumContainer(
			    atts.namedItem(maxstr).nodeValue(), cs, this
			   );
			 */
		}
	}
	m_container = cont;
	m_spinBox->setValue(m_container->value());
	// connect
	connect(m_container, SIGNAL(valueChanged(int)), m_spinBox, SLOT(setValue(int)));
	connect(m_spinBox, SIGNAL(valueChanged(int)), this, SLOT(informChange(int)));
}
void InputConditionWidgetInteger::setMaximum(const QVariant& value)
{
	m_spinBox->setMaximum(value.toInt());
}
void InputConditionWidgetInteger::setMinimum(const QVariant& value)
{
	m_spinBox->setMinimum(value.toInt());
}

void InputConditionWidgetInteger::informChange(int value)
{
	m_container->setValue(value);
}
