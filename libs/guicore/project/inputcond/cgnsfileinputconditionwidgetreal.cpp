#include "../../solverdef/solverdefinitiontranslator.h"
#include "cgnsfileinputconditioncontainerreal.h"
#include "cgnsfileinputconditionwidgetreal.h"

#include <guibase/realnumbereditwidget.h>

#include <QHBoxLayout>
#include <QMessageBox>

CgnsFileInputConditionWidgetReal::CgnsFileInputConditionWidgetReal(QDomNode defnode, const SolverDefinitionTranslator& /*t*/, CgnsFileInputConditionContainerReal* cont) : CgnsFileInputConditionWidget(defnode)
{
	m_lineEdit = new RealNumberEditWidget(this);
	m_lineEdit->setMinimumWidth(100);
	m_lineEdit->setAlignment(Qt::AlignRight);
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addStretch(1);
	layout->setSpacing(0);
	layout->addWidget(m_lineEdit, 1);
	layout->setMargin(CgnsFileInputConditionWidget::margin);
	setLayout(layout);
	// @todo min, max is not regarded yet.
	m_Container = cont;
	setValue(cont->value());
	connect(m_lineEdit, SIGNAL(valueChanged(double)), this, SLOT(getWidgetValue(double)));
	connect(m_Container, SIGNAL(valueChanged(double)), this, SLOT(setValue(double)));
}
void CgnsFileInputConditionWidgetReal::setValue(double v)
{
	m_lineEdit->setValue(v);
}

void CgnsFileInputConditionWidgetReal::getWidgetValue(double v)
{
	m_Container->setValue(v);
}

void CgnsFileInputConditionWidgetReal::setDisabled(bool disable)
{
	m_lineEdit->setDisabled(disable);
}
