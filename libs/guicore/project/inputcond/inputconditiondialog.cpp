#include "ui_inputconditiondialog.h"

#include "../../misc/cgnsfileopener.h"
#include "../../solverdef/solverdefinition.h"
#include "../../solverdef/solverdefinitiontranslator.h"
#include "../projectcgnsfile.h"
#include "inputconditioncontainerset.h"
#include "inputconditiondialog.h"
#include "inputconditionwidgetset.h"
#include "inputconditionwidgetset.h"

#include <misc/filesystemfunction.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>
#include <misc/versionnumber.h>

#include <QDir>
#include <QDomDocument>
#include <QDomNode>
#include <QFile>
#include <QMessageBox>
#include <QPushButton>

#include <cgnslib.h>
#include <iriclib.h>

InputConditionDialog::InputConditionDialog(SolverDefinition* solverDef, const QLocale& locale, QWidget* parent) :
	QDialog(parent),
	m_containerSet {new InputConditionContainerSet()},
	m_containerSetBackup {nullptr},
	m_widgetSet {new InputConditionWidgetSet()},
	m_solverDefinition {solverDef},
	m_modified {false},
	m_readonly {false},
	ui {new Ui::InputConditionDialog}
{
	ui->setupUi(this);
	connect(m_containerSet, SIGNAL(modified()), this, SLOT(setModified()));
	// create connections.
	connect(ui->m_pageList, SIGNAL(selectChanged(QString)),
					ui->m_pageContainer, SLOT(pageSelected(QString)));
	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)),
					this, SLOT(handleButtonClick(QAbstractButton*)));

	setup(*solverDef, locale);

	ui->splitter->setStretchFactor(0, 0.3);
	ui->buttonBox->button(QDialogButtonBox::Save)->setText(InputConditionDialog::tr("&Save and Close"));
}

InputConditionDialog::~InputConditionDialog()
{
	delete ui;
	m_containerSet->clear();
	delete m_containerSet;
	m_containerSetBackup->clear();
	delete m_containerSetBackup;
	m_widgetSet->clear();
	delete m_widgetSet;
}

void InputConditionDialog::changeEvent(QEvent* e)
{
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}
void InputConditionDialog::setup(const SolverDefinition& def, const QLocale& locale)
{
	// open solve definition file
	SolverDefinitionTranslator t(def.folder().absolutePath(), locale);
	QDomNode condNode = iRIC::getChildNode(def.document().documentElement(), "CalculationCondition");
	// setup ContainerSet first.
	m_containerSet->setup(condNode, def, t);
	m_containerSetBackup = m_containerSet->clone();
	// setup WidgetSet.
	m_widgetSet->setup(condNode, *m_containerSet, def, t);
	// setup PageList.
	ui->m_pageList->setup(condNode.toElement(), t);
	// setup PageContainer.
	ui->m_pageContainer->setup(condNode.toElement(), m_widgetSet, t);
	// select the first page.
	ui->m_pageList->selectFirstItem();
}

void InputConditionDialog::load(const int fn)
{
	cg_iRIC_GotoCC(fn);
	m_containerSet->load();
	m_containerSetBackup->copyValues(m_containerSet);

	// select the first page.
	ui->m_pageList->selectFirstItem();
	m_modified = false;
}

void InputConditionDialog::save(const int fn)
{
	cg_iRIC_GotoCC(fn);
	m_containerSet->save();
	m_containerSetBackup->copyValues(m_containerSet);
	m_modified = false;
}

bool InputConditionDialog::importFromCgns(const QString& filename)
{
	// load from the specified file.
	int fn, ret;
	QString tmpname = iRIC::getTempFileName(m_workFolder);
	// Copy to a temporary file.
	bool bret = QFile::copy(filename, tmpname);
	if (! bret) {return false;}

	// Check the compatibility.
	std::string solverName;
	VersionNumber versionNumber;
	bret = ProjectCgnsFile::readSolverInfo(tmpname, &solverName, &versionNumber);

	if (bret == true) {
		if (m_solverDefinition->name() != solverName || (! m_solverDefinition->version().compatibleWith(versionNumber))) {
			QMessageBox::critical(parentWidget(), tr("Error"),
					tr("This CGNS file is created for %1 version %2. It is not compatible with the current solver.").arg(solverName.c_str()).arg(versionNumber.toString()));
			return false;
		}
	} else {
		// error occured reading solver information.
		int ret = QMessageBox::warning(parentWidget(), tr("Warning"),
																	 tr("This CGNS file does not have solver information. "
																			"We can not check whether this CGNS file is compatible with the solver. If it is not compatible, maybe only some part of the conditions will be imported.\nDo you really want to import this file?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (ret == QMessageBox::No) {return false;}
	}

	// open cgns file
	ret = cg_open(iRIC::toStr(tmpname).c_str(), CG_MODE_READ, &fn);
	if (ret != 0) {return false;}

	ret = cg_iRIC_GotoCC(fn);
	if (ret != 0) {goto ERROR_BEFORE_CLOSE;}
	// check the number of user defined data items under the CalculationConditions node.
	int nuser_data;
	ret = cg_nuser_data(&nuser_data);
	if (ret != 0) {goto ERROR_BEFORE_CLOSE;}
	if (nuser_data == 0) {
		// there is no calculation data in this CGNS file.
		QMessageBox::critical(parentWidget(), tr("Error"),
													tr("This CGNS file does not contain calculation condition data."));
		goto ERROR_BEFORE_CLOSE;
	}
	ret = m_containerSet->load();
	if (ret != 0) {goto ERROR_BEFORE_CLOSE;}

	ret = cg_close(fn);
	if (ret != 0) {return false;}

	m_modified = false;
	QFile::remove(tmpname);
	return true;

ERROR_BEFORE_CLOSE:
	cg_close(fn);
	QFile::remove(tmpname);
	return false;
}

bool InputConditionDialog::importFromYaml(const QString& filename)
{
	return m_containerSet->importFromYaml(filename);
}

bool InputConditionDialog::exportToCgns(const QString& filename)
{
	// Create an empty CGNS file first. Use temporary name,
	// because cg_open() does not supports file names with Non-ASCII characters.
	QString tmpname = iRIC::getTempFileName(m_workFolder);

	bool bret = ProjectCgnsFile::createNewFile(tmpname, 2, 2);
	if (! bret) {return false;}
	bret = ProjectCgnsFile::writeSolverInfo(tmpname, &(m_solverDefinition->abstract()));
	if (! bret) {return false;}
	// Save into the specified file.
	int fn, ret;
	// Open cgns file
	ret = cg_open(iRIC::toStr(tmpname).c_str(), CG_MODE_MODIFY, &fn);
	if (ret != 0) {return false;}
	ret = cg_iRIC_GotoCC(fn);
	if (ret != 0) {return false;}
	// Save calculation condition.
	m_containerSet->save();
	ret = cg_close(fn);
	if (ret != 0) {return false;}

	// Saved into temporary file.
	// Copy into the specified file.
	if (QFile::exists(filename)) {
		// this file already exists. remove it first.
		bret = QFile::remove(filename);
		if (! bret) {return false;}
	}
	bret = QFile::copy(tmpname, filename);
	if (! bret) {return false;}
	// Delete the temporary file.
	bret = QFile::remove(tmpname);
	if (! bret) {return false;}
	return true;
}

bool InputConditionDialog::exportToYaml(const QString& filename)
{
	return m_containerSet->exportToYaml(filename);
}

void InputConditionDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole) {
		reset();
	}
}

void InputConditionDialog::reset()
{
	int ret = QMessageBox::warning(this, tr("Warning"), tr("Are you sure you want to reset all calculation conditions to default values?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (ret == QMessageBox::No) {return;}
	m_containerSet->reset();
}

void InputConditionDialog::accept()
{
	if (m_readonly) {
		QMessageBox::critical(parentWidget(), tr("Error"), tr("The solver is running currently, so you can not save calculation condition. Please press Cancel button."));
		return;
	}
	QDialog::accept();
}

void InputConditionDialog::reject()
{
	if (m_modified && (QMessageBox::Cancel == QMessageBox::warning(this, tr("Warning"), tr("Modifications you made will be discarded."), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel))) {
		return;
	}
	m_containerSet->copyValues(m_containerSetBackup);
	m_modified = false;
	QDialog::reject();
}

void InputConditionDialog::setModified()
{
	m_modified = true;
}

void InputConditionDialog::setReadOnly(bool readonly)
{
	m_readonly = readonly;
	m_widgetSet->toggleReadOnly(readonly);
	if (readonly) {
		ui->buttonBox->button(QDialogButtonBox::Save)->setDisabled(true);
		ui->buttonBox->button(QDialogButtonBox::Reset)->setDisabled(true);
	} else {
		ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(true);
		ui->buttonBox->button(QDialogButtonBox::Reset)->setEnabled(true);
	}
}

void InputConditionDialog::checkImportSourceUpdate()
{
	bool ret = m_widgetSet->checkImportSourceUpdate();

	if (! ret) {return;}

	int fn, ier;
	ier = cg_open(iRIC::toStr(m_fileName).c_str(), CG_MODE_MODIFY, &fn);
	if (ier != 0) {
		QMessageBox::critical(parentWidget(), tr("Error"), tr("Error occured while saving."));
		return;
	}
	save(fn);
	cg_close(fn);
}
