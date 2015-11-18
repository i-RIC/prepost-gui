#include "../../solverdef/solverdefinitiontranslator.h"
#include "inputconditioncontainerstring.h"
#include "inputconditionwidgetfilename.h"

#include <guibase/asciionlylineedit.h>

#include <QFileDialog>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>

QString InputConditionWidgetFilename::defaultFolder;

InputConditionWidgetFilename::InputConditionWidgetFilename(QDomNode defNode, const SolverDefinitionTranslator&, InputConditionContainerString* cont) :
	InputConditionWidget(defNode),
	m_all (false)
{
	if (defNode.toElement().attribute("valueType") == "filename_all"){
		m_all = true;
	}else{
		m_all = false;
	}

	m_container = cont;

	m_lineEdit = new AsciiOnlyLineEdit(this);
	m_lineEdit->setErrorMessage(tr("File name has to consist of only English characters."));

	QPushButton* button = new QPushButton(QString(tr("...")), this);
	button->setFixedWidth(30);
	button->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

	QHBoxLayout * layout = new QHBoxLayout(0);
	layout->setMargin(InputConditionWidget::margin);
	layout->addWidget(m_lineEdit, 10);
	layout->addWidget(button, 0);
	setLayout(layout);

	setValue(cont->value());

	connect(m_lineEdit, SIGNAL(editingFinished()), this, SLOT(handleEditingFinished()));
	connect(button, SIGNAL(clicked(bool)), this, SLOT(openFileDialog()));
	connect(m_container, SIGNAL(valueChanged(QString)), this, SLOT(setValue(QString)));
}

void InputConditionWidgetFilename::setValue(const QString& newvalue)
{
	m_lineEdit->setText(newvalue);
}

void InputConditionWidgetFilename::handleEditingFinished()
{
	m_container->setValue(m_lineEdit->text());
}

void InputConditionWidgetFilename::openFileDialog()
{
	// Get dir from the current text.
	QString dir;
	QString originalFile = QDir::fromNativeSeparators(m_lineEdit->text());
	if (! originalFile.isEmpty()){
		QFileInfo finfo(originalFile);
		if (finfo.isAbsolute()){
			dir = finfo.absolutePath();
		}
	}
	if (! QFile::exists(dir)){
		dir = defaultFolder;
	}

	// Select file name withdialog
	QString fname;
	if (m_all){
		fname = QFileDialog::getSaveFileName(this, tr("Select File"), dir);
	} else {
		fname = QFileDialog::getOpenFileName(this, tr("Select File"), dir);
	}
	if (fname.isEmpty()) {
		// canceled.
		return;
	}

	m_lineEdit->setText(QDir::toNativeSeparators(fname));
	handleEditingFinished();

	QFileInfo finfo(fname);
	defaultFolder = finfo.absolutePath();
}
