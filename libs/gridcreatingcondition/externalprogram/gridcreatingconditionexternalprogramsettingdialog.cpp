#include "ui_gridcreatingconditionexternalprogramsettingdialog.h"

#include "gridcreatingconditionexternalprogramsettingdialog.h"

#include <guicore/base/iricmainwindowinterface.h>
#include <guicore/project/inputcond/inputconditioncontainerset.h>
#include <guicore/project/inputcond/inputconditionwidgetset.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitiontranslator.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QDir>
#include <QDomDocument>
#include <QDomNode>
#include <QMessageBox>
#include <QPushButton>

#include <h5cgnsbase.h>
#include <h5cgnsfile.h>

GridCreatingConditionExternalProgramSettingDialog::GridCreatingConditionExternalProgramSettingDialog(SolverDefinition* def, const QLocale& locale, iRICMainWindowInterface* /*mainW*/, QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionExternalProgramSettingDialog)
{
	ui->setupUi(this);
	m_containerSet = new InputConditionContainerSet();
	m_widgetSet = new InputConditionWidgetSet();
	// create connections.
	connect(ui->m_pageList, SIGNAL(selectChanged(QString)),
		ui->m_pageContainer, SLOT(pageSelected(QString)));
	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)),
		this, SLOT(handleButtonClick(QAbstractButton*)));

	setup(*def, locale);

	ui->buttonBox->button(QDialogButtonBox::Save)->setText(tr("&Create Grid"));
}

GridCreatingConditionExternalProgramSettingDialog::~GridCreatingConditionExternalProgramSettingDialog()
{
	delete ui;
	m_containerSet->clear();
	delete m_containerSet;

	m_widgetSet->clear();
	delete m_widgetSet;
}

void GridCreatingConditionExternalProgramSettingDialog::setup(const SolverDefinition& def, const QLocale& locale)
{
	// open solve definition file
	SolverDefinitionTranslator t(def.folder().absolutePath(), locale);
	QDomNode condNode = iRIC::getChildNode(def.document().documentElement(), "GridGeneratingCondition");
	// setup ContainerSet first.
	m_containerSet->setup(condNode, def, t);
	// setup WidgetSet.
	m_widgetSet->setup(condNode, *m_containerSet, def, t);
	// setup PageList.
	ui->m_pageList->setup(condNode.toElement(), t);
	// setup PageContainer.
	ui->m_pageContainer->setup(condNode.toElement(), m_widgetSet, t);
	// select the first page.
	ui->m_pageList->selectFirstItem();

	// setup errormessages.
	QDomNode ecsNode = iRIC::getChildNode(def.document().documentElement(), "ErrorCodes");
	if (! ecsNode.isNull()) {
		for (int i = 0; i < ecsNode.childNodes().count(); ++i) {
			QDomNode ecNode = ecsNode.childNodes().at(i);
			QDomElement ecElem = ecNode.toElement();
			int errorCode = ecElem.attribute("value").toInt();
			QString errorMessage = t.translate(ecElem.attribute("caption"));
			m_errorMessages.insert(errorCode, errorMessage);
		}
	}
}

bool GridCreatingConditionExternalProgramSettingDialog::load()
{
	try {
		iRICLib::H5CgnsFile file(iRIC::toStr(m_filename), iRICLib::H5CgnsFile::Mode::OpenReadOnly);
		auto ccGroup = file.ccBase()->ccGroup();
		m_containerSet->load(*ccGroup);

		// select the first page.
		ui->m_pageList->selectFirstItem();
		return true;
	} catch (...) {
		return false;
	}
}

bool GridCreatingConditionExternalProgramSettingDialog::save()
{
	try {
		iRICLib::H5CgnsFile file(iRIC::toStr(m_filename), iRICLib::H5CgnsFile::Mode::Create);
		m_containerSet->save(file.ccBase()->ccGroup());
		return true;
	}  catch (...) {
		return false;
	}
}

void GridCreatingConditionExternalProgramSettingDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole) {
		reset();
	}
}

void GridCreatingConditionExternalProgramSettingDialog::reset(bool nodialog)
{
	if (! nodialog) {
		int ret = QMessageBox::warning(this, tr("Warning"), tr("Are you sure you want to reset all grid creating conditions to default values?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
		if (ret == QMessageBox::No) {return;}
	}
	m_containerSet->reset();
}

void GridCreatingConditionExternalProgramSettingDialog::accept()
{
	bool ok = save();
	if (! ok) {
		QMessageBox::critical(parentWidget(), tr("Error"), tr("Error occured while saving."));
	}

	QDialog::accept();
}

void GridCreatingConditionExternalProgramSettingDialog::setFilename(const QString& filename)
{
	m_filename = filename;
}

QString GridCreatingConditionExternalProgramSettingDialog::errorMessage(int errorcode) const
{
	if (m_errorMessages.contains(errorcode)) {
		return m_errorMessages.value(errorcode);
	} else {
		return QString(tr("Unknown error (Error code: %1)")).arg(errorcode);
	}
}
