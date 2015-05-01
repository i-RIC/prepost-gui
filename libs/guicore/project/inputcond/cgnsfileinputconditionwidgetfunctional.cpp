#include "../../solverdef/solverdefinitiontranslator.h"
#include "cgnsfileinputconditioncontainerfunctional.h"
#include "cgnsfileinputconditionwidgetfunctional.h"
#include "functional/cgnsfileinputconditionwidgetfunctionaldialog.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QPushButton>

CgnsFileInputConditionWidgetFunctional::CgnsFileInputConditionWidgetFunctional(QDomNode defnode, const SolverDefinitionTranslator& t, CgnsFileInputConditionContainerFunctional* cont) : CgnsFileInputConditionWidget(defnode)
{
	m_container = cont;
	QPushButton* button = new QPushButton(QString(tr("Edit")), this);
	button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	QHBoxLayout* l = new QHBoxLayout(0);
	l->addStretch(1);
	l->addWidget(button, 0);
	l->setMargin(CgnsFileInputConditionWidget::margin);
	setLayout(l);
	connect(button, SIGNAL(clicked()), this, SLOT(openDialog()));

	m_dialog = new CgnsFileInputConditionWidgetFunctionalDialog(defnode, t, this);
	connect(m_dialog, SIGNAL(accepted()), this, SLOT(dialogAccepted()));
}

CgnsFileInputConditionWidgetFunctional::~CgnsFileInputConditionWidgetFunctional() {}

void CgnsFileInputConditionWidgetFunctional::openDialog()
{
	m_dialog->setData(*m_container);
	m_dialog->setModal(true);
	m_dialog->show();
}

void CgnsFileInputConditionWidgetFunctional::dialogAccepted()
{
	*m_container = m_dialog->container();
}
