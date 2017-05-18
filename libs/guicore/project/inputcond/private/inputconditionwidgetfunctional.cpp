#include "../inputconditioncontainerfunctional.h"
#include "../../../solverdef/solverdefinitiontranslator.h"
#include "inputconditionwidgetfunctionaldialog.h"
#include "inputconditionwidgetfunctional.h"
#include "inputconditionwidgettooltip.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QPushButton>

InputConditionWidgetFunctional::InputConditionWidgetFunctional(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerFunctional* cont) :
	InputConditionWidget(defnode),
	m_container {cont}
{
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

InputConditionWidgetFunctional::~InputConditionWidgetFunctional()
{}

void InputConditionWidgetFunctional::setCaption(const QString& caption)
{
	m_dialog->setWindowTitle(caption);
}

void InputConditionWidgetFunctional::addTooltip(const QString& tooltip)
{
	InputConditionWidgetTooltip* tt = new InputConditionWidgetTooltip(tooltip, this);

	QHBoxLayout* l = dynamic_cast<QHBoxLayout*>(layout());
	l->insertWidget(1, tt);
}

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
