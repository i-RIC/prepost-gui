#include "geodatapolydatagroupmergesettingdialog.h"
#include "ui_geodatapolydatagroupmergesettingdialog.h"

#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
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

void GeoDataPolyDataGroupMergeSettingDialog::setItems(const std::vector<PreProcessorGeoDataDataItemInterface*>& items)
{
	for (PreProcessorGeoDataDataItemInterface* item : items) {
		ui->listWidget->addItem(item->geoData()->caption());
	}
	m_items = items;
}

std::vector<PreProcessorGeoDataDataItemInterface*> GeoDataPolyDataGroupMergeSettingDialog::selectedItems() const
{
	std::vector<PreProcessorGeoDataDataItemInterface*> ret;
	for (int i = 0; i < ui->listWidget->count(); ++i) {
		auto item = ui->listWidget->item(i);
		if (! item->isSelected()) {continue;}

		ret.push_back(m_items.at(i));
	}
	return ret;
}
