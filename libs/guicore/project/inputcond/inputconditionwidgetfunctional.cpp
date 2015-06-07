#include "../../solverdef/solverdefinitiontranslator.h"
#include "inputconditioncontainerfunctional.h"
#include "inputconditionwidgetfunctional.h"
#include "functional/inputconditionwidgetfunctionaldialog.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QPushButton>

InputConditionWidgetFunctional::InputConditionWidgetFunctional(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerFunctional* cont) : InputConditionWidget(defnode)
{
	m_container = cont;
	QPushButton* button = new QPushButton(QString(tr("Edit")), this);
	button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	QHBoxLayout* l = new QHBoxLayout(0);
	l->addStretch(1);
	l->addWidget(button, 0);
	l->setMargin(InputConditionWidget::margin);
	setLayout(l);
	connect(button, SIGNAL(clicked()), this, SLOT(openDialog()));

	m_dialog = new InputConditionWidgetFunctionalDialog(defnode, t, this);
	connect(m_dialog, SIGNAL(accepted()), this, SLOT(dialogAccepted()));
}

InputConditionWidgetFunctional::~InputConditionWidgetFunctional() {}

void InputConditionWidgetFunctional::openDialog()
{
	m_dialog->setData(*m_container);
	m_dialog->setModal(true);
	m_dialog->show();
}

void InputConditionWidgetFunctional::dialogAccepted()
{
	*m_container = m_dialog->container();
}
