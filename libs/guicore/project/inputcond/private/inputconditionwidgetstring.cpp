#include "../../../solverdef/solverdefinitiontranslator.h"
#include "../inputconditioncontainerstring.h"
#include "inputconditionwidgetstring.h"

#include <guibase/asciionlylineedit.h>

#include <QHBoxLayout>
#include <QMessageBox>
#include <QTextCodec>

InputConditionWidgetString::InputConditionWidgetString(QDomNode defnode, const SolverDefinitionTranslator& /*t*/, InputConditionContainerString* cont) : InputConditionWidget(defnode)
{
	m_container = cont;

	m_lineEdit = new AsciiOnlyLineEdit(this);
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setMargin(InputConditionWidget::margin);
	layout->addWidget(m_lineEdit, 1);
	setLayout(layout);

	setValue(cont->value());

	connect(m_lineEdit, SIGNAL(editingFinished()), this, SLOT(handleEditingFinished()));
	connect(m_container, SIGNAL(valueChanged(QString)), this, SLOT(setValue(QString)));
}

void InputConditionWidgetString::setValue(const QString& newvalue)
{
	m_lineEdit->setText(newvalue);
}

void InputConditionWidgetString::handleEditingFinished()
{
	m_container->setValue(m_lineEdit->text());
}
