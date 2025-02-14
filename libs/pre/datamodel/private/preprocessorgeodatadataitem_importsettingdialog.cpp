#include "preprocessorgeodatadataitem_importsettingdialog.h"
#include "ui_preprocessorgeodatadataitem_importsettingdialog.h"

namespace {
	const int NAME_WIDTH = 110;
	const int MARGIN = 40;
}

PreProcessorGeoDataDataItem::ImportSettingDialog::ImportSettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PreProcessorGeoDataDataItem_ImportSettingDialog)
{
	ui->setupUi(this);
}

PreProcessorGeoDataDataItem::ImportSettingDialog::~ImportSettingDialog()
{
	delete ui;
}

void PreProcessorGeoDataDataItem::ImportSettingDialog::setItems(const std::vector<GeoDataImporterSetting::Item>& items)
{
	ui->tableWidget->setColumnCount(2);
	ui->tableWidget->setRowCount(static_cast<int> (items.size()));
	ui->tableWidget->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Name")));
	ui->tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem(tr("Value")));

	int row = 0;
	for (const auto& item : items) {
		ui->tableWidget->setItem(row, 0, new QTableWidgetItem(item.name));
		ui->tableWidget->setItem(row, 1, new QTableWidgetItem(item.value));
		++ row;
	}
	ui->tableWidget->setColumnWidth(0, NAME_WIDTH);
	ui->tableWidget->setColumnWidth(1, width() - NAME_WIDTH - MARGIN);
}
