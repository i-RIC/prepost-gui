#include "solverdefinitionlist.h"
#include "solverdefinitiontranslationchecker.h"
#include "solverdefinitiontranslationupdatewizard.h"

#include <gridcreatingcondition/externalprogram/gridcreatingconditioncreatorexternalprogram.h>
#include <guicore/pre/gridcreatingcondition/gridcreatingconditioncreator.h>
#include <guicore/solverdef/solverdefinitionabstract.h>
#include <guicore/solverdef/solverdefinitiontranslator.h>

#include <QCheckBox>
#include <QComboBox>
#include <QDir>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QRadioButton>
#include <QScrollArea>
#include <QTextEdit>
#include <QVBoxLayout>

SolverDefinitionTranslationUpdateWizard::SolverDefinitionTranslationUpdateWizard(QWidget* parent) :
	QWizard(parent)
{
	addPage(new IntroductionPage(this));
	m_settingPageId = addPage(new SettingPage(this));
	addPage(new ConfirmPage(this));
	setWindowTitle(tr("Definition File Translation Update Wizard"));
	resize(560, 500);
}

void SolverDefinitionTranslationUpdateWizard::init(SolverDefinitionList* list, const QList<GridCreatingConditionCreator*>& gclist)
{
	m_solverList = list;
	m_gridCreatorList = gclist;
	QWizardPage* p = page(m_settingPageId);
	dynamic_cast<SettingPage*>(p)->init(list, gclist);
}

void SolverDefinitionTranslationUpdateWizard::accept()
{
	QDir folderDir;
	if (field("solverSel").toBool() == true) {
		auto solver = solverList()->solverList().at(field("solver").toInt());
		folderDir = QDir(solver->folder());
	} else if (field("gridGenSel").toBool() == true) {
		auto creator = gridCreatorList().at(field("gridgen").toInt());
		GridCreatingConditionCreatorExternalProgram* pc = dynamic_cast<GridCreatingConditionCreatorExternalProgram*>(creator);
		folderDir = QDir(pc->folderName());
	}
	QList<QLocale> allLangs = SolverDefinitionTranslationUpdateWizard::supportedLanguages();
	for (const QLocale& locale : allLangs) {
		QVariant val = field(QString("lang%1").arg(locale.name()));
		if (val.toBool() == true) {
			// update or create the translation file.
			SolverDefinitionTranslationChecker checker(folderDir.absolutePath(), locale);
		}
	}
	QWizard::accept();
}

IntroductionPage::IntroductionPage(QWidget* parent)
	: QWizardPage(parent)
{
	setTitle(tr("Introduction"));
	m_label = new QLabel(tr(
			"This function is intended to be used by solver developers or grid creating "
			"program developers, not users.\n"
			"This wizard will generate or update translation files for solver definition files and grid creating program definition files "
			"those are registered to iRIC system. "
			"You need to specify the solver (or grid creating program) name, and the list of languages "
			"for those you want to create or update translation files."
				));
	m_label->setWordWrap(true);
	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(m_label);
	setLayout(layout);
}

SettingPage::SettingPage(QWidget* parent)
	: QWizardPage(parent)
{
	setTitle(tr("Select Program and Languages"));
	setSubTitle(tr(
			"Specify for which solver (or grid creating program) you want to create or update translation files."
				));
	QVBoxLayout* layout = new QVBoxLayout(this);

	QGroupBox* targetGroupBox = new QGroupBox(tr("Target Program"), this);

	m_solverRadioButton = new QRadioButton(tr("&Solver"), targetGroupBox);
	m_solverRadioButton->setChecked(true);
	registerField("solverSel", m_solverRadioButton);
	m_gridGenRadioButton = new QRadioButton(tr("&Grid Creating Program"), targetGroupBox);
	registerField("gridGenSel", m_gridGenRadioButton);

	m_solverCombobox = new QComboBox(targetGroupBox);
	registerField("solver", m_solverCombobox);
	m_gridGenComboBox = new QComboBox(targetGroupBox);
	m_gridGenComboBox->setDisabled(true);
	registerField("gridgen", m_gridGenComboBox);

	connect(m_solverRadioButton, SIGNAL(toggled(bool)), m_solverCombobox, SLOT(setEnabled(bool)));
	connect(m_gridGenRadioButton, SIGNAL(toggled(bool)), m_gridGenComboBox, SLOT(setEnabled(bool)));

	QGridLayout* gLayout = new QGridLayout(targetGroupBox);
	gLayout->addWidget(m_solverRadioButton, 0, 0, 1, 1);
	gLayout->addWidget(m_solverCombobox, 0, 1, 1, 1);
	gLayout->addWidget(m_gridGenRadioButton, 1, 0, 1, 1);
	gLayout->addWidget(m_gridGenComboBox, 1, 1, 1, 1);
	targetGroupBox->setLayout(gLayout);

	layout->addWidget(targetGroupBox);
	QList<QLocale> langs = SolverDefinitionTranslationUpdateWizard::supportedLanguages();
	QGroupBox* langGroupBox = new QGroupBox(tr("Languages"), this);

	QScrollArea* langsArea = new QScrollArea(langGroupBox);
	QWidget* langsWidget = new QWidget(langsArea);
	//langsArea->setWidget(langsWidget);
	QVBoxLayout* llayout = new QVBoxLayout(langsWidget);
	// llayout->setSizeConstraint(QLayout::SetMinimumSize);
	for (const QLocale& locale : langs) {
		QCheckBox* check = new QCheckBox(langsWidget);
		check->setText(QLocale::languageToString(locale.language()));
		registerField(QString("lang%1").arg(locale.name()), check);
		llayout->addWidget(check);
	}
	llayout->addStretch(1);
	langsWidget->setLayout(llayout);

	QVBoxLayout* langLayout = new QVBoxLayout(langGroupBox);
	langsArea->setWidget(langsWidget);

	langLayout->addWidget(langsArea);
	langGroupBox->setLayout(langLayout);

	layout->addWidget(targetGroupBox);
	layout->addWidget(langGroupBox, 1);

	setLayout(layout);
}

QList<QLocale> SolverDefinitionTranslationUpdateWizard::supportedLanguages()
{
	QList<QLocale> ret;
	// The list of supported languages are shown here.
	// Please insert all the languages that iRIC may support.

	ret.append(QLocale(QLocale::Arabic));
	ret.append(QLocale(QLocale::Basque));
	ret.append(QLocale(QLocale::Bosnian));
	ret.append(QLocale(QLocale::Bulgarian));
	ret.append(QLocale(QLocale::Catalan));
	ret.append(QLocale(QLocale::Chinese, QLocale::China));
	ret.append(QLocale(QLocale::Chinese, QLocale::Taiwan));
	ret.append(QLocale(QLocale::Czech));
	ret.append(QLocale(QLocale::Danish));
	ret.append(QLocale(QLocale::Dutch));
	ret.append(QLocale(QLocale::Estonian));
	ret.append(QLocale(QLocale::Finnish));
	ret.append(QLocale(QLocale::French));
	ret.append(QLocale(QLocale::German));
	ret.append(QLocale(QLocale::Galician));
	ret.append(QLocale(QLocale::Greek));
	ret.append(QLocale(QLocale::Hindi));
	ret.append(QLocale(QLocale::Hungarian));
	ret.append(QLocale(QLocale::Icelandic));
	ret.append(QLocale(QLocale::Indonesian));
	ret.append(QLocale(QLocale::Italian));
	ret.append(QLocale(QLocale::Japanese));
	ret.append(QLocale(QLocale::Korean));
	ret.append(QLocale(QLocale::Latvian));
	ret.append(QLocale(QLocale::Lithuanian));
	ret.append(QLocale(QLocale::Norwegian));
	ret.append(QLocale(QLocale::Polish));
	ret.append(QLocale(QLocale::Portuguese, QLocale::Brazil));
	ret.append(QLocale(QLocale::Portuguese, QLocale::Portugal));
	ret.append(QLocale(QLocale::Romanian));
	ret.append(QLocale(QLocale::Russian));
	ret.append(QLocale(QLocale::Slovenian));
	ret.append(QLocale(QLocale::Spanish));
	ret.append(QLocale(QLocale::Swedish));
	ret.append(QLocale(QLocale::Thai));
	ret.append(QLocale(QLocale::Turkish));
	ret.append(QLocale(QLocale::Ukrainian));

	return ret;
}

void SettingPage::init(SolverDefinitionList* list, const QList<GridCreatingConditionCreator*>& gclist)
{
	for (SolverDefinitionAbstract* abst : list->solverList()) {
		m_solverCombobox->addItem(abst->caption(), abst->name().c_str());
	}

	for (GridCreatingConditionCreator* c : gclist) {
		m_gridGenComboBox->addItem(c->caption(), c->name());
	}
}

void SettingPage::initializePage()
{
	SolverDefinitionTranslationUpdateWizard* w = dynamic_cast<SolverDefinitionTranslationUpdateWizard*>(wizard());
	if (field("solverSel").toBool() == true) {
		m_solverRadioButton->setChecked(true);
		int solverId = field("solver").toInt();
		if (solverId == -1) {solverId = 0;}
		m_solverCombobox->setCurrentIndex(solverId);
		m_gridGenComboBox->setCurrentIndex(0);
	} else if (field("gridGenSel").toBool() == true) {
		m_gridGenRadioButton->setChecked(true);
		int creatorId = field("gridgen").toInt();
		if (creatorId == -1) {creatorId = 0;}
		m_gridGenComboBox->setCurrentIndex(creatorId);
		m_solverCombobox->setCurrentIndex(0);
	}
}

ConfirmPage::ConfirmPage(QWidget* /*parent*/)
{
	setTitle(tr("Confirm the result"));
	setSubTitle(tr(
			"The files in the list below will be created or updated. "
			"Please open these files with Qt Linguist, "
			"and complete the translation dictionary."
				));

	m_textEdit = new QTextEdit(this);
	m_textEdit->setReadOnly(true);
	m_textEdit->setAcceptRichText(false);

	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(m_textEdit);
	setLayout(layout);
}

void ConfirmPage::initializePage()
{
	SolverDefinitionTranslationUpdateWizard* w = dynamic_cast<SolverDefinitionTranslationUpdateWizard*>(wizard());
	QDir folderDir;
	if (field("solverSel").toBool() == true) {
		auto solver = w->solverList()->solverList().at(field("solver").toInt());
		folderDir = QDir(solver->folder());
	} else if (field("gridGenSel").toBool() == true) {
		auto creator = w->gridCreatorList().at(field("gridgen").toInt());
		GridCreatingConditionCreatorExternalProgram* pc = dynamic_cast<GridCreatingConditionCreatorExternalProgram*>(creator);
		folderDir = QDir(pc->folderName());
	}
	QList<QLocale> allLangs = SolverDefinitionTranslationUpdateWizard::supportedLanguages();
	QString filelist;
	for (const QLocale& locale : allLangs) {
		QVariant val = field(QString("lang%1").arg(locale.name()));
		if (val.toBool() == true) {
			QString fname = SolverDefinitionTranslator::filenameFromLocale(locale);
			filelist.append(QDir::toNativeSeparators(folderDir.absoluteFilePath(fname))).append("\n");
		}
	}
	m_textEdit->setText(filelist);
}
