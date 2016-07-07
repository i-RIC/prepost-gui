#include "../../../solverdef/solverdefinitiontranslator.h"
#include "../inputconditioncontainerstring.h"
#include "inputconditionwidgetfoldername.h"

#include <guibase/widget/asciionlylineedit.h>

#include <QFileDialog>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>

QString InputConditionWidgetFoldername::defaultFolder;

InputConditionWidgetFoldername::InputConditionWidgetFoldername(QDomNode defnode, const SolverDefinitionTranslator& /*t*/, InputConditionContainerString* cont) :
	InputConditionWidget(defnode)
{
	m_container = cont;

	m_lineEdit = new AsciiOnlyLineEdit(this);
	m_lineEdit->setErrorMessage(tr("Folder name has to consist of only English characters."));

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
	connect(button, SIGNAL(clicked(bool)), this, SLOT(openDirDialog()));
	connect(m_container, SIGNAL(valueChanged(QString)), this, SLOT(setValue(QString)));
}

void InputConditionWidgetFoldername::setValue(const QString& newvalue)
{
	m_lineEdit->setText(newvalue);
}

void InputConditionWidgetFoldername::handleEditingFinished()
{
	m_container->setValue(m_lineEdit->text());
}

void InputConditionWidgetFoldername::openDirDialog(){
	// Get dir from the current text.
	QString dir;
	QString originalFile = QDir::fromNativeSeparators(m_lineEdit->text());
	if (! originalFile.isEmpty()){
		dir = originalFile;
	}
	if (! QFile::exists(dir)){
		dir = defaultFolder;
	}
	QString dirname = QFileDialog::getExistingDirectory(this, tr("Select Directory"), dir);
	if (dirname.isEmpty()) {
		// canceled.
		return;
	}

	m_lineEdit->setText(QDir::toNativeSeparators(dirname), true);
	handleEditingFinished();

	defaultFolder = dirname;
}
