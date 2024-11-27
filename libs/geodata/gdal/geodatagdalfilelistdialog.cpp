#include "geodatanetcdffilelistdialog.h"
#include "geodatanetcdffilenamematcher.h"
#include "ui_geodatanetcdffilelistdialog.h"

#include <QDir>

GeoDataNetcdfFileListDialog::GeoDataNetcdfFileListDialog(QWidget *parent) :
	QDialog(parent),
	m_fileNameMatcher {nullptr},
	ui(new Ui::GeoDataNetcdfFileListDialog)
{
	ui->setupUi(this);
	connect(ui->selectAllButton, SIGNAL(clicked()), this, SLOT(selectAll()));
	connect(ui->deselectAllButton, SIGNAL(clicked()), this, SLOT(deselectAll()));
}

GeoDataNetcdfFileListDialog::~GeoDataNetcdfFileListDialog()
{
	delete ui;
}

void GeoDataNetcdfFileListDialog::setFileNameMatcher(GeoDataNetcdfFileNameMatcher* matcher)
{
	m_fileNameMatcher = matcher;
}

void GeoDataNetcdfFileListDialog::setFileNames(const std::vector<QString>& fileNames)
{
	m_fileNames = fileNames;

	for (const QString& fname : fileNames) {
		ui->listWidget->addItem(QFileInfo(fname).fileName());
	}
	ui->listWidget->selectAll();
}

std::vector<QString> GeoDataNetcdfFileListDialog::selectedFilenames() const
{
	std::vector<QString> ret;

	for (int i = 0; i < ui->listWidget->count(); ++i) {
		auto item = ui->listWidget->item(i);
		if (! item->isSelected()) {continue;}

		ret.push_back(m_fileNames.at(i));
	}

	return ret;
}

void GeoDataNetcdfFileListDialog::accept()
{
	auto filenames = selectedFilenames();

	bool ok;
	if (filenames.size() > 2) {
		// calculate the first diff
		auto firstDt = m_fileNameMatcher->getDateTime(filenames.at(0), &ok);
		auto secondDt = m_fileNameMatcher->getDateTime(filenames.at(1), &ok);
		auto diffMsecs = firstDt.msecsTo(secondDt);

		bool allSameDistance = true;

		for (int i = 1; i < filenames.size() - 1; ++i) {
			auto dt1 = m_fileNameMatcher->getDateTime(filenames.at(i), &ok);
			auto dt2 = m_fileNameMatcher->getDateTime(filenames.at(i + 1), &ok);
			auto diffMsecs2 = dt1.msecsTo(dt2);

			allSameDistance = allSameDistance && (diffMsecs2 == diffMsecs);
		}

		if (! allSameDistance) {
			int ret = QMessageBox::warning(this, tr("Warning"), tr("The time interval between the selected files are not the same."), QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
			if (ret == QMessageBox::Cancel) {return;}
		}
	}

	QDialog::accept();
}

void GeoDataNetcdfFileListDialog::selectAll()
{
	ui->listWidget->selectAll();
}

void GeoDataNetcdfFileListDialog::deselectAll()
{
	ui->listWidget->clearSelection();
}
