#include "abstractcrosssectionwindow_csvexportcontroller.h"
#include "abstractcrosssectionwindow_exportsettingdialog.h"

#include <guibase/widget/waitdialog.h>
#include <misc/filesystemfunction.h>
#include <misc/lastiodirectory.h>

#include <QDir>
#include <QMessageBox>

AbstractCrosssectionWindow::CsvExportController::CsvExportController(AbstractCrosssectionWindow* window) :
	m_dirName {LastIODirectory::get()},
	m_prefix {"crosssection_"},
	m_mode {ExportSettingDialog::CrossSectionMode::Current},
	m_min {0},
	m_max {0},
	m_skipRate {1},
	m_canceled {false},
	m_window {window}
{}

void AbstractCrosssectionWindow::CsvExportController::save()
{
	m_canceled = false;
	auto current = m_window->targetIndex();

	ExportSettingDialog dialog(m_window);
	dialog.setWindowTitle(tr("Export CSV Files"));
	dialog.setDirName(m_dirName);
	dialog.setPrefix(m_prefix);
	dialog.setMode(m_mode);
	if (m_mode == ExportSettingDialog::CrossSectionMode::Custom) {
		dialog.setMin(m_min);
		dialog.setMax(m_max);
	}
	dialog.setSkipRate(m_skipRate);
	int ret = dialog.exec();

	if (ret == QDialog::Rejected) {return;}

	m_dirName = dialog.dirName();
	m_prefix = dialog.prefix();
	m_mode = dialog.mode();
	m_min = dialog.min();
	m_max = dialog.max();
	m_skipRate = dialog.skipRate();

	if (! iRIC::mkdirRecursively(m_dirName)) {
		QMessageBox::critical(m_window, AbstractCrosssectionWindow::tr("Error"), AbstractCrosssectionWindow::tr("Error occured while making %1.").arg(QDir::toNativeSeparators(m_dirName)));
		return;
	}
	QDir dir(m_dirName);

	WaitDialog waitDialog(m_window);
	waitDialog.setRange(m_min, m_max);
	connect(&waitDialog, &WaitDialog::canceled, this, &CsvExportController::cancel);

	if (m_min != m_max) {
		waitDialog.show();
	}

	for (int i = dialog.min(); i < dialog.max(); i += m_skipRate) {
		m_window->setTarget(m_window->targetDirection(), i);

		auto fname = dir.absoluteFilePath(QString("%1%2.csv").arg(m_prefix).arg(i + 1));
		m_window->saveCsvFile(fname);
		qApp->processEvents();

		if (m_canceled) {break;}
	}

	m_window->setTarget(m_window->targetDirection(), current);
}

void AbstractCrosssectionWindow::CsvExportController::cancel()
{
	m_canceled = true;
}
