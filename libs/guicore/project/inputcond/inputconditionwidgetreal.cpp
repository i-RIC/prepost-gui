#include "../../solverdef/solverdefinitiontranslator.h"
#include "inputconditioncontainerreal.h"
#include "inputconditionwidgetreal.h"

#include <guibase/realnumbereditwidget.h>

#include <QHBoxLayout>
#include <QMessageBox>

InputConditionWidgetReal::InputConditionWidgetReal(QDomNode defnode, const SolverDefinitionTranslator& /*t*/, InputConditionContainerReal* cont) : InputConditionWidget(defnode)
{
	m_lineEdit = new RealNumberEditWidget(this);
	m_lineEdit->setMinimumWidth(100);
	m_lineEdit->setAlignment(Qt::AlignRight);
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addStretch(1);
	layout->setSpacing(0);
	layout->addWidget(m_lineEdit, 1);
	layout->setMargin(InputConditionWidget::margin);
	setLayout(layout);
	// @todo min, max is not regarded yet.
	m_Container = cont;
	setValue(cont->value());
	connect(m_lineEdit, SIGNAL(valueChanged(double)), this, SLOT(getWidgetValue(double)));
	connect(m_Container, SIGNAL(valueChanged(double)), this, SLOT(setValue(double)));
}
void InputConditionWidgetReal::setValue(double v)
{
	m_lineEdit->setValue(v);
}

void InputConditionWidgetReal::getWidgetValue(double v)
{
	m_Container->setValue(v);
}

void InputConditionWidgetReal::setDisabled(bool disable)
{
	m_lineEdit->setDisabled(disable);
}
