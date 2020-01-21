#include "geodatanetcdffilelistdialog.h"
#include "ui_geodatanetcdffilelistdialog.h"

#include <QDir>

GeoDataNetcdfFileListDialog::GeoDataNetcdfFileListDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GeoDataNetcdfFileListDialog)
{
	ui->setupUi(this);
}

GeoDataNetcdfFileListDialog::~GeoDataNetcdfFileListDialog()
{
	delete ui;
}

void GeoDataNetcdfFileListDialog::setFilenames(const std::vector<QString>& filenames)
{
	for (const QString& fname : filenames) {
		ui->listWidget->addItem(QDir::toNativeSeparators(fname));
	}
}
