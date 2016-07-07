#include "../../../solverdef/solverdefinitiontranslator.h"
#include "../inputconditioncontainerstring.h"
#include "inputconditionwidgetstring.h"

#include <guibase/widget/asciionlylineedit.h>
#include <guibase/widget/asciionlytextedit.h>

#include <QHBoxLayout>
#include <QMessageBox>
#include <QTextCodec>

InputConditionWidgetString::InputConditionWidgetString(QDomNode defnode, const SolverDefinitionTranslator& /*t*/, InputConditionContainerString* cont) : InputConditionWidget(defnode)
{
	m_container = cont;

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setMargin(InputConditionWidget::margin);
	if (defnode.toElement().attribute("multiline") == "true") {
		AsciiOnlyTextEdit* te = new AsciiOnlyTextEdit(this);
		connect(te, SIGNAL(textChanged()), this, SLOT(handleEditingFinished()));
		layout->addWidget(te, 1);
		m_textEdit = te;
	}	else {
		AsciiOnlyLineEdit* te = new AsciiOnlyLineEdit(this);
		connect(te, SIGNAL(editingFinished()), this, SLOT(handleEditingFinished()));
		layout->addWidget(te, 1);
		m_textEdit = te;
	}
	setLayout(layout);

	setValue(cont->value());

	connect(m_container, SIGNAL(valueChanged(QString)), this, SLOT(setValue(QString)));
}

void InputConditionWidgetString::setValue(const QString& newvalue)
{
	if (newvalue == m_textEdit->text()) {return;}

	m_textEdit->setText(newvalue);
}

void InputConditionWidgetString::handleEditingFinished()
{
	m_container->setValue(m_textEdit->text());
}
