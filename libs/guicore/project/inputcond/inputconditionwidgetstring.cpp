#include "../../solverdef/solverdefinitiontranslator.h"
#include "inputconditioncontainerstring.h"
#include "inputconditionwidgetstring.h"

#include <QHBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QTextCodec>

InputConditionWidgetString::InputConditionWidgetString(QDomNode defnode, const SolverDefinitionTranslator& /*t*/, InputConditionContainerString* cont) : InputConditionWidget(defnode)
{
	m_container = cont;
	m_lineEdit = new QLineEdit(this);
	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->setMargin(InputConditionWidget::margin);
	layout->addWidget(m_lineEdit, 1);
	setLayout(layout);
	setValue(cont->value());
	connect(m_lineEdit, SIGNAL(editingFinished()), this, SLOT(checkContent()));
	connect(m_container, SIGNAL(valueChanged(const QString&)), m_lineEdit, SLOT(setText(QString)));
	connect(m_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(getWidgetValue(const QString&)));
}

void InputConditionWidgetString::checkContent()
{
	QString txt = m_lineEdit->text();
	QTextCodec* asciiCodec = QTextCodec::codecForName("latin1");
	bool ok = asciiCodec->canEncode(txt);
	if (! ok) {
		QMessageBox::warning(this, tr("Warning"), tr("String has to consist of only English characters."));
		m_lineEdit->setText("");
	}
}

void InputConditionWidgetString::setDisabled(bool disable)
{
	m_lineEdit->setDisabled(disable);
}

void InputConditionWidgetString::setValue(const QString& newvalue)
{
	m_lineEdit->setText(newvalue);
}

void InputConditionWidgetString::getWidgetValue(const QString& newvalue)
{
	m_container->setValue(newvalue);
}
