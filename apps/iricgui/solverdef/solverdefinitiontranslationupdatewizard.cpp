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

SolverDefinitionTranslationUpdateWizard::SolverDefinitionTranslationUpdateWizard(QWidget* parent)
	: QWizard(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	addPage(new IntroductionPage(this));
	m_settingPageId = addPage(new SettingPage(this));
	addPage(new ConfirmPage(this));
	setWindowTitle(tr("Definition File Translation Update Wizard"));
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
	QList<SolverDefinitionAbstract*> slist = solverList()->solverList();
	const QList<GridCreatingConditionCreator*>& clist = gridCreatorList();

	QDir folderDir;
	if (field("solverSel").toBool() == true){
		SolverDefinitionAbstract* solver = slist.at(field("solver").toInt());
		folderDir = QDir(solver->folder());
	} else if (field("gridGenSel").toBool() == true){
		GridCreatingConditionCreator* creator = clist.at(field("gridgen").toInt());
		GridCreatingConditionCreatorExternalProgram* pc = dynamic_cast<GridCreatingConditionCreatorExternalProgram*>(creator);
		folderDir = QDir(pc->folderName());
	}
	QList<QLocale> allLangs = SolverDefinitionTranslationUpdateWizard::supportedLanguages();
	QList<QLocale>::iterator lit;
	QString filelist;
	for (lit = allLangs.begin(); lit != allLangs.end(); ++lit){
		QLocale locale = *lit;
		QVariant val = field(QString("lang%1").arg(locale.name()));
		if (val.toBool() == true){
			// update or create the translation file.
			SolverDefinitionTranslationChecker checker(folderDir.absolutePath(), locale);
		}
	}
	QWizard::accept();
}

IntroductionPage::IntroductionPage(QWidget *parent)
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
	QVBoxLayout *layout = new QVBoxLayout();
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
	QList<QLocale>::iterator it;
	QGroupBox* langGroupBox = new QGroupBox(tr("Languages"), this);

	QScrollArea* langsArea = new QScrollArea();
	langsArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	QVBoxLayout* llayout = new QVBoxLayout();
    llayout->setSizeConstraint(QLayout::SetMinimumSize);
	for (it = langs.begin(); it != langs.end(); ++it){
		QCheckBox* check = new QCheckBox(this);
		QLocale locale = *it;
		check->setText(QLocale::languageToString(locale.language()));
		registerField(QString("lang%1").arg(locale.name()), check);
		llayout->addWidget(check);
	}
	llayout->addStretch(1);
	langsArea->setLayout(llayout);
	QVBoxLayout* langLayout = new QVBoxLayout(langGroupBox);
	langLayout->addWidget(langsArea);
	langGroupBox->setLayout(langLayout);

	layout->addWidget(targetGroupBox);
	layout->addWidget(langGroupBox);
	layout->addStretch(1);

	setLayout(layout);
}

QList<QLocale> SolverDefinitionTranslationUpdateWizard::supportedLanguages()
{
	QList<QLocale> ret;
	// The list of supported languages are shown here.
	// Please insert all the languages that iRIC may support.
	ret.append(QLocale::Japanese);
    ret.append(QLocale::Korean);
    ret.append(QLocale::Thai);
    ret.append(QLocale::Indonesian);
    ret.append(QLocale::Chinese);
	ret.append(QLocale::French);
	ret.append(QLocale::Spanish);
	ret.append(QLocale::Russian);
/*
	ret.append(QLocale::Italian);
	ret.append(QLocale::German);
*/
	return ret;
}

void SettingPage::init(SolverDefinitionList *list, const QList<GridCreatingConditionCreator*>& gclist)
{
	QList<SolverDefinitionAbstract*> slist = list->solverList();
	QList<SolverDefinitionAbstract*>::iterator it;
	for (it = slist.begin(); it != slist.end(); ++it){
		SolverDefinitionAbstract* abst = *it;
		m_solverCombobox->addItem(abst->caption(), abst->name());
	}

	QList<GridCreatingConditionCreator*>::const_iterator git;
	for (git = gclist.begin(); git != gclist.end(); ++git){
		GridCreatingConditionCreator* c = *git;
		m_gridGenComboBox->addItem(c->caption(), c->name());
	}
}

void SettingPage::initializePage()
{
	SolverDefinitionTranslationUpdateWizard* w = dynamic_cast<SolverDefinitionTranslationUpdateWizard*>(wizard());
	QList<SolverDefinitionAbstract*> slist = w->solverList()->solverList();
	const QList<GridCreatingConditionCreator*>& clist = w->gridCreatorList();
	QDir folderDir;
	if (field("solverSel").toBool() == true){
		m_solverRadioButton->setChecked(true);
		int solverId = field("solver").toInt();
		if (solverId == -1){solverId = 0;}
		m_solverCombobox->setCurrentIndex(solverId);
		m_gridGenComboBox->setCurrentIndex(0);
	} else if (field("gridGenSel").toBool() == true){
		m_gridGenRadioButton->setChecked(true);
		int creatorId = field("gridgen").toInt();
		if (creatorId == -1){creatorId = 0;}
		m_gridGenComboBox->setCurrentIndex(creatorId);
		m_solverCombobox->setCurrentIndex(0);
	}
}

ConfirmPage::ConfirmPage(QWidget * /*parent*/)
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
	QList<SolverDefinitionAbstract*> slist = w->solverList()->solverList();
	const QList<GridCreatingConditionCreator*>& clist = w->gridCreatorList();
	QDir folderDir;
	if (field("solverSel").toBool() == true){
		SolverDefinitionAbstract* solver = slist.at(field("solver").toInt());
		folderDir = QDir(solver->folder());
	} else if (field("gridGenSel").toBool() == true){
		GridCreatingConditionCreator* creator = clist.at(field("gridgen").toInt());
		GridCreatingConditionCreatorExternalProgram* pc = dynamic_cast<GridCreatingConditionCreatorExternalProgram*>(creator);
		folderDir = QDir(pc->folderName());
	}
	QList<QLocale> allLangs = SolverDefinitionTranslationUpdateWizard::supportedLanguages();
	QList<QLocale>::iterator lit;
	QString filelist;
	for (lit = allLangs.begin(); lit != allLangs.end(); ++lit){
		QLocale locale = *lit;
		QVariant val = field(QString("lang%1").arg(locale.name()));
		if (val.toBool() == true){
			QString fname = SolverDefinitionTranslator::filenameFromLocale(locale);
            filelist.append(QDir::toNativeSeparators(folderDir.absoluteFilePath(fname))).append("\n");
		}
	}
	m_textEdit->setText(filelist);
}
