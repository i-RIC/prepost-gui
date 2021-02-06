
#include "inputconditioncgnsfile.h"
#include "inputconditionwidgetcgnsfile.h"
#include "inputconditionwidgettooltip.h"
#include "../inputconditioncontainerstring.h"
#include "../inputconditionwidgetfilename.h"
#include "ui_inputconditionwidgetcgnsfile.h"

#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>

InputConditionWidgetCgnsFile::InputConditionWidgetCgnsFile(QDomNode defnode, const SolverDefinitionTranslator& t, InputConditionContainerString* cont, InputConditionCgnsFile* file) :
	InputConditionWidget(defnode),
	m_container {cont},
	m_cgnsFile {file},
	ui(new Ui::InputConditionWidgetCgnsFile)
{
	ui->setupUi(this);
	ui->filenameEdit->setErrorMessage(tr("File name has to consist of only English characters."));

	setValue(cont->value());
	connect(ui->filenameEdit, SIGNAL(editingFinished()), this, SLOT(handleEditingFinished()));
	connect(ui->openDialogButton, SIGNAL(clicked(bool)), this, SLOT(openFileDialog()));
	connect(m_container, SIGNAL(valueChanged(QString)), this, SLOT(setValue(QString)));
	connect(file, SIGNAL(changed()), this, SLOT(handleFileChange()));
}

InputConditionWidgetCgnsFile::~InputConditionWidgetCgnsFile()
{
	delete ui;
}

void InputConditionWidgetCgnsFile::addTooltip(const QString& tooltip)
{
	InputConditionWidgetTooltip* tt = new InputConditionWidgetTooltip(tooltip, this);

	QHBoxLayout* l = dynamic_cast<QHBoxLayout*>(ui->nameEditLayout);
	l->insertWidget(0, tt);
}

void InputConditionWidgetCgnsFile::setValue(const QString& newvalue)
{
	ui->filenameEdit->setText(newvalue);
	informChanged();
}

void InputConditionWidgetCgnsFile::handleEditingFinished()
{
	m_container->setValue(ui->filenameEdit->text());
	informChanged();
}

void InputConditionWidgetCgnsFile::handleFileChange()
{
	if (m_cgnsFile->isEffective()) {
		const auto& info = m_cgnsFile->solverInformation();
		ui->solverNameValueLabel->setText(info.solverName);
		ui->solverVersionValueLabel->setText(info.solverVersion);
		if (info.gridType == InputConditionCgnsFile::SolverInformation::GridType::Structured) {
			ui->gridTypeValueLabel->setText(tr("Structured"));
			ui->gridSizeValueLabel->setText(QString("%1 x %2 = %3").arg(info.iSize).arg(info.jSize).arg(info.iSize * info.jSize));
		} else {
			ui->gridTypeValueLabel->setText(tr("Unstructured"));
			ui->gridSizeValueLabel->setText(QString("%1").arg(info.size));
		}
		ui->resultNumValueLabel->setText(QString::number(info.timeSteps));
	} else {
		QString NO_DATA = "---";
		ui->solverNameValueLabel->setText(NO_DATA);
		ui->solverVersionValueLabel->setText(NO_DATA);
		ui->gridTypeValueLabel->setText(NO_DATA);
		ui->gridSizeValueLabel->setText(NO_DATA);
		ui->resultNumValueLabel->setText(NO_DATA);
	}
}

void InputConditionWidgetCgnsFile::openFileDialog()
{
	// Get dir from the current text.
	QString dir;
	QString originalFile = QDir::fromNativeSeparators(ui->filenameEdit->text());
	if (! originalFile.isEmpty()){
		QFileInfo finfo(originalFile);
		if (finfo.isAbsolute()){
			dir = finfo.absolutePath();
		}
	}
	if (! QFile::exists(dir)){
		dir = InputConditionWidgetFilename::defaultFolder;
	}

	// Select file name withdialog
	QString fname;
	fname = QFileDialog::getOpenFileName(this, tr("Select File"), dir);

	if (fname.isEmpty()) {
		// canceled.
		return;
	}

	ui->filenameEdit->setText(QDir::toNativeSeparators(fname), true);
	handleEditingFinished();

	QFileInfo finfo(fname);
	InputConditionWidgetFilename::defaultFolder = finfo.absolutePath();
}

void InputConditionWidgetCgnsFile::informChanged()
{
	m_cgnsFile->setFileName(m_container->value());
}
