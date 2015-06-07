#include "../../solverdef/solverdefinitiontranslator.h"
#include "inputconditioncontainerstring.h"
#include "inputconditionwidgetfoldername.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTextCodec>

QString CgnsFileInputConditionWidgetFoldername::defaultFolder;

bool InputConditionWidgetFoldernameEdit::checkContent()
{
	QString txt = text();
	QTextCodec* asciiCodec = QTextCodec::codecForName("latin1");
	bool ok = asciiCodec->canEncode(txt);
	if (! ok) {
		QMessageBox::warning(this, tr("Warning"), tr("Folder name has to consist of only English characters."));
		setText("");
	}
	return ok;
}

void InputConditionWidgetFoldernameEdit::focusOutEvent(QFocusEvent* e)
{
	checkContent();

	emit valueChanged(text());
	QLineEdit::focusOutEvent(e);
}

CgnsFileInputConditionWidgetFoldername::CgnsFileInputConditionWidgetFoldername(QDomNode defnode, const SolverDefinitionTranslator& /*t*/, InputConditionContainerString* cont) : InputConditionWidget(defnode)
{
	m_container = cont;
	connect(m_container, SIGNAL(valueChanged(QString)), this, SLOT(setValue(QString)));

	m_edit = new InputConditionWidgetFoldernameEdit(this);
	QPushButton* button = new QPushButton(QString(tr("...")), this);
	button->setFixedWidth(30);
	button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
	// Should setup QLineEdit and a button;
	QHBoxLayout* l = new QHBoxLayout(0);
	l->setMargin(InputConditionWidget::margin);
	l->addWidget(m_edit, 10);
	l->addWidget(button, 0);
	setLayout(l);
	setValue(cont->value());
	connect(m_edit, SIGNAL(textChanged(QString)), this, SLOT(getWidgetValue(const QString&)));
	connect(button, SIGNAL(clicked()), this, SLOT(openDirDialog()));
}

void CgnsFileInputConditionWidgetFoldername::openDirDialog()
{
	QString originalFile = QDir::fromNativeSeparators(m_edit->text());
	QString dir;
	if (! originalFile.isEmpty()) {
		dir = originalFile;
	}
	if (! QFile::exists(dir)) {
		dir = defaultFolder;
	}
	QString dirname = QFileDialog::getExistingDirectory(this, tr("Select Directory"), dir);
	if (! dirname.isNull()) {
		m_edit->setText(QDir::toNativeSeparators(dirname));
		m_edit->checkContent();
		defaultFolder = dirname;
	}
}

void CgnsFileInputConditionWidgetFoldername::getWidgetValue(const QString& newvalue)
{
	m_container->setValue(newvalue);
}
