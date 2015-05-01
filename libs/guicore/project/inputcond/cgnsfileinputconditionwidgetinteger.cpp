#include "cgnsfileinputconditionwidgetinteger.h"
#include "cgnsfileinputconditioncontainerinteger.h"
#include "cgnsfileinputconditioncontainerset.h"
#include "../../solverdef/solverdefinitiontranslator.h"

//#include "../Dependency/ccdMisc.h"

#include <QSpinBox>
#include <QHBoxLayout>
#include <QDomElement>

CgnsFileInputConditionWidgetInteger::CgnsFileInputConditionWidgetInteger(QDomNode defnode, const SolverDefinitionTranslator& /*t*/, CgnsFileInputConditionContainerInteger* cont, const CgnsFileInputConditionContainerSet& /*cs*/) : CgnsFileInputConditionWidget(defnode)
{
	m_spinBox = new QSpinBox(this);
	m_spinBox->setMinimumWidth(100);
	m_spinBox->setAlignment(Qt::AlignRight);
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addStretch(1);
	layout->addWidget(m_spinBox, 1);
	layout->setMargin(CgnsFileInputConditionWidget::margin);
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
void CgnsFileInputConditionWidgetInteger::setMaximum(const QVariant& value)
{
	m_spinBox->setMaximum(value.toInt());
}
void CgnsFileInputConditionWidgetInteger::setMinimum(const QVariant& value)
{
	m_spinBox->setMinimum(value.toInt());
}

void CgnsFileInputConditionWidgetInteger::informChange(int value)
{
	m_container->setValue(value);
}
