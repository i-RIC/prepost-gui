#include "ui_preferencepagegeneral.h"

#include "preferencepagegeneral.h"
#include "../misc/periodicalupdatechecker.h"

#include <guicore/project/projectworkspace.h>
#include <guicore/pre/grid/grid.h>
#include <solverconsole/solverconsolewindowprojectdataitem.h>
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

	bool checkPeriodically = settings.value("general/periodicalUpdate", true).toBool();
	ui->checkUpdatePeriodicallyCheckBox->setChecked(checkPeriodically);
	int checkInterval = settings.value("general/periodicalUpdateInterval", PeriodicalUpdateChecker::DEFAULT_INTERVAL_DAYS).toInt();
	ui->checkIntervalSpinBox->setValue(checkInterval);

	bool cullEnable = settings.value("general/cullcellenable", true).toBool();
	int cullCellLimit = settings.value("general/cullcelllimit", Grid::MAX_DRAWCELLCOUNT).toInt();
	int cullIndexLimit = settings.value("general/cullindexlimit", Grid::MAX_DRAWINDEXCOUNT).toInt();
	ui->cullEnableCheckBox->setChecked(cullEnable);
	ui->cullMaxNumberSpinBox->setValue(cullCellLimit);
	ui->cullMaxIndexSpinBox->setValue(cullIndexLimit);

	int undoLimit = settings.value("general/undolimit", iRICUndoStack::DEFAULT_UNDOLIMIT).toInt();
	ui->undoLimitSpinBox->setValue(undoLimit);

	int maxLines = settings.value("general/scMaxLines", SolverConsoleWindowProjectDataItem::MAXLINES).toInt();
	ui->maxLinesSpinBox->setValue(maxLines);
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
	settings.setValue("general/periodicalUpdate", ui->checkUpdatePeriodicallyCheckBox->isChecked());
	settings.setValue("general/periodicalUpdateInterval", ui->checkIntervalSpinBox->value());
	settings.setValue("general/cullcellenable", ui->cullEnableCheckBox->isChecked());
	settings.setValue("general/cullcelllimit", ui->cullMaxNumberSpinBox->value());
	settings.setValue("general/cullindexlimit", ui->cullMaxIndexSpinBox->value());
	int undoLimit = ui->undoLimitSpinBox->value();
	settings.setValue("general/undolimit", undoLimit);
	iRICUndoStack::instance().setUndoLimit(undoLimit);
	settings.setValue("general/scMaxLines", ui->maxLinesSpinBox->value());
}

void PreferencePageGeneral::setupLanguageComboBox()
{
	QComboBox* combo = ui->languageComboBox;

	m_locales.append(QLocale(QLocale::Arabic));
	m_locales.append(QLocale(QLocale::Basque));
	m_locales.append(QLocale(QLocale::Bosnian));
	m_locales.append(QLocale(QLocale::Bulgarian));
	m_locales.append(QLocale(QLocale::Catalan));
	m_locales.append(QLocale(QLocale::Chinese, QLocale::China));
	m_locales.append(QLocale(QLocale::Chinese, QLocale::Taiwan));
	m_locales.append(QLocale(QLocale::Czech));
	m_locales.append(QLocale(QLocale::Danish));
	m_locales.append(QLocale(QLocale::Dutch));
	m_locales.append(QLocale(QLocale::Estonian));
	m_locales.append(QLocale(QLocale::Finnish));
	m_locales.append(QLocale(QLocale::French));
	m_locales.append(QLocale(QLocale::German));
	m_locales.append(QLocale(QLocale::Galician));
	m_locales.append(QLocale(QLocale::Greek));
	m_locales.append(QLocale(QLocale::Hindi));
	m_locales.append(QLocale(QLocale::Hungarian));
	m_locales.append(QLocale(QLocale::Icelandic));
	m_locales.append(QLocale(QLocale::Indonesian));
	m_locales.append(QLocale(QLocale::Italian));
	m_locales.append(QLocale(QLocale::Japanese));
	m_locales.append(QLocale(QLocale::Korean));
	m_locales.append(QLocale(QLocale::Latvian));
	m_locales.append(QLocale(QLocale::Lithuanian));
	m_locales.append(QLocale(QLocale::Norwegian));
	m_locales.append(QLocale(QLocale::Polish));
	m_locales.append(QLocale(QLocale::Portuguese, QLocale::Brazil));
	m_locales.append(QLocale(QLocale::Portuguese, QLocale::Portugal));
	m_locales.append(QLocale(QLocale::Romanian));
	m_locales.append(QLocale(QLocale::Russian));
	m_locales.append(QLocale(QLocale::Slovenian));
	m_locales.append(QLocale(QLocale::Spanish));
	m_locales.append(QLocale(QLocale::Swedish));
	m_locales.append(QLocale(QLocale::Thai));
	m_locales.append(QLocale(QLocale::Turkish));
	m_locales.append(QLocale(QLocale::Ukrainian));

	for (QLocale loc : m_locales) {
		QString str;
		if (loc.language() == QLocale::Portuguese || loc.language() == QLocale::Chinese) {
			str = QString("%1 (%2)").arg(QLocale::languageToString(loc.language())).arg(QLocale::countryToString(loc.country()));
		} else {
			str = QLocale::languageToString(loc.language());
		}
		combo->addItem(str, QVariant(loc.name()));
	}
}

QString PreferencePageGeneral::defaultWorkspace() const
{
	QDir homeDir = QDir::home();
	return homeDir.absoluteFilePath(ProjectWorkspace::FOLDERNAME);
}
