#include "ui_preferencepagegeneral.h"

#include "preferencepagegeneral.h"

#include <guicore/project/projectworkspace.h>
#include <guicore/pre/grid/grid.h>
#include <misc/stringtool.h>
#include <misc/iricundostack.h>

#include <QDir>
#include <QList>
#include <QLocale>
#include <QMessageBox>
#include <QSettings>

PreferencePageGeneral::PreferencePageGeneral(QWidget* parent) :
	PreferencePage(parent),
	ui(new Ui::PreferencePageGeneral)
{
	ui->setupUi(this);
	setupLanguageComboBox();

	QSettings settings;
	QString locale = settings.value("general/locale", QLocale::system().name()).value<QString>();
	QLocale currentLocale(locale);
	int locIndex = m_locales.indexOf(currentLocale);
	if (locIndex != -1) {
		ui->languageComboBox->setCurrentIndex(locIndex);
	}
	QString workspace = settings.value("general/workspace", defaultWorkspace()).value<QString>();
	ui->workDirWidget->setDirname(workspace);
	bool copyFolderProject = settings.value("general/copyfolderproject", true).toBool();
	ui->folderCopyCheckBox->setChecked(copyFolderProject);

	bool cullEnable = settings.value("general/cullcellenable", true).toBool();
	int cullCellLimit = settings.value("general/cullcelllimit", Grid::MAX_DRAWCELLCOUNT).toInt();
	int cullIndexLimit = settings.value("general/cullindexlimit", Grid::MAX_DRAWINDEXCOUNT).toInt();
	ui->cullEnableCheckBox->setChecked(cullEnable);
	ui->cullMaxNumberSpinBox->setValue(cullCellLimit);
	ui->cullMaxIndexSpinBox->setValue(cullIndexLimit);

	int undoLimit = settings.value("general/undolimit", iRICUndoStack::DEFAULT_UNDOLIMIT).toInt();
	ui->undoLimitSpinBox->setValue(undoLimit);
}

PreferencePageGeneral::~PreferencePageGeneral()
{
	delete ui;
}

bool PreferencePageGeneral::checkSetting()
{
	QString workDir = ui->workDirWidget->dirname();
	if (! iRIC::isAscii(workDir)) {
		QMessageBox::critical(this, tr("Error"), tr("Working directory contains non-ASCII characters."));
		return false;
	}
	return true;
}

void PreferencePageGeneral::update()
{
	QSettings settings;
	QString oldWorkspace = settings.value("general/workspace", defaultWorkspace()).value<QString>();
	QString newWorkspace = ui->workDirWidget->dirname();
	if (oldWorkspace != newWorkspace) {
		QMessageBox::warning(this, tr("Warning"), tr(
													 "Working directory is changed, but the new working directory is used AFTER exiting iRIC GUI!"));
	}
	int locIndex = ui->languageComboBox->currentIndex();
	QLocale loc = m_locales.at(locIndex);
	settings.setValue("general/locale", loc.name());
	settings.setValue("general/workspace", ui->workDirWidget->dirname());
	settings.setValue("general/copyfolderproject", ui->folderCopyCheckBox->isChecked());
	settings.setValue("general/cullcellenable", ui->cullEnableCheckBox->isChecked());
	settings.setValue("general/cullcelllimit", ui->cullMaxNumberSpinBox->value());
	settings.setValue("general/cullindexlimit", ui->cullMaxIndexSpinBox->value());
	int undoLimit = ui->undoLimitSpinBox->value();
	settings.setValue("general/undolimit", undoLimit);
	iRICUndoStack::instance().setUndoLimit(undoLimit);
}

void PreferencePageGeneral::setupLanguageComboBox()
{
	QComboBox* combo = ui->languageComboBox;

	m_locales.append(QLocale(QLocale::English));
	m_locales.append(QLocale(QLocale::Japanese));
	m_locales.append(QLocale(QLocale::Korean));
	m_locales.append(QLocale(QLocale::Thai));
	m_locales.append(QLocale(QLocale::Indonesian));
	m_locales.append(QLocale(QLocale::Chinese));
	m_locales.append(QLocale(QLocale::French));
	m_locales.append(QLocale(QLocale::Spanish));
	m_locales.append(QLocale(QLocale::Russian));

	for (QLocale loc : m_locales) {
		combo->addItem(QLocale::languageToString(loc.language()), QVariant(loc.name()));
	}
}

QString PreferencePageGeneral::defaultWorkspace() const
{
	QDir homeDir = QDir::home();
	return homeDir.absoluteFilePath(ProjectWorkspace::FOLDERNAME);
}
