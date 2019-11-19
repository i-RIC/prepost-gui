#include "geodatapolygongroupmergesettingdialog.h"
#include "ui_geodatapolygongroupmergesettingdialog.h"

#include <guicore/pre/base/preprocessorgeodatadataiteminterface.h>
#include <guicore/pre/geodata/geodata.h>

GeoDataPolygonGroupMergeSettingDialog::GeoDataPolygonGroupMergeSettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GeoDataPolygonGroupMergeSettingDialog)
{
	ui->setupUi(this);

	connect(ui->selectAllButton, SIGNAL(clicked()), ui->listWidget, SLOT(selectAll()));
	connect(ui->unselectAllButton, SIGNAL(clicked()), ui->listWidget, SLOT(clearSelection()));
}

GeoDataPolygonGroupMergeSettingDialog::~GeoDataPolygonGroupMergeSettingDialog()
{
	delete ui;
}

void GeoDataPolygonGroupMergeSettingDialog::setItems(const std::vector<PreProcessorGeoDataDataItemInterface*>& items)
{
	for (PreProcessorGeoDataDataItemInterface* item : items) {
		ui->listWidget->addItem(item->geoData()->caption());
	}
	m_items = items;
}

std::vector<PreProcessorGeoDataDataItemInterface*> GeoDataPolygonGroupMergeSettingDialog::selectedItems() const
{
	std::vector<PreProcessorGeoDataDataItemInterface*> ret;
	for (int i = 0; i < ui->listWidget->count(); ++i) {
		auto item = ui->listWidget->item(i);
		if (! item->isSelected()) {continue;}

		ret.push_back(m_items.at(i));
	}
	return ret;
}
