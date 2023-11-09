#include "geodatapolydatagroupmergesettingdialog.h"
#include "ui_geodatapolydatagroupmergesettingdialog.h"

#include <guicore/pre/base/preprocessorgeodatadataitemi.h>
#include <guicore/pre/geodata/geodata.h>

GeoDataPolyDataGroupMergeSettingDialog::GeoDataPolyDataGroupMergeSettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GeoDataPolyDataGroupMergeSettingDialog)
{
	ui->setupUi(this);

	connect(ui->selectAllButton, SIGNAL(clicked()), ui->listWidget, SLOT(selectAll()));
	connect(ui->unselectAllButton, SIGNAL(clicked()), ui->listWidget, SLOT(clearSelection()));
}

GeoDataPolyDataGroupMergeSettingDialog::~GeoDataPolyDataGroupMergeSettingDialog()
{
	delete ui;
}

void GeoDataPolyDataGroupMergeSettingDialog::setItems(const std::vector<PreProcessorGeoDataDataItemI*>& items)
{
	for (PreProcessorGeoDataDataItemI* item : items) {
		ui->listWidget->addItem(item->geoData()->caption());
	}
	m_items = items;
}

std::vector<PreProcessorGeoDataDataItemI*> GeoDataPolyDataGroupMergeSettingDialog::selectedItems() const
{
	std::vector<PreProcessorGeoDataDataItemI*> ret;
	for (int i = 0; i < ui->listWidget->count(); ++i) {
		auto item = ui->listWidget->item(i);
		if (! item->isSelected()) {continue;}

		ret.push_back(m_items.at(i));
	}
	return ret;
}
