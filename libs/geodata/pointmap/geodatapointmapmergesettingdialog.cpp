#include "geodatapointmapmergesettingdialog.h"
#include "ui_geodatapointmapmergesettingdialog.h"

#include <guicore/pre/base/preprocessorgeodatadataitemi.h>
#include <guicore/pre/geodata/geodata.h>

GeoDataPointmapMergeSettingDialog::GeoDataPointmapMergeSettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GeoDataPointmapMergeSettingDialog)
{
	ui->setupUi(this);

	connect(ui->selectAllButton, SIGNAL(clicked()), ui->listWidget, SLOT(selectAll()));
	connect(ui->unselectAllButton, SIGNAL(clicked()), ui->listWidget, SLOT(clearSelection()));
}

GeoDataPointmapMergeSettingDialog::~GeoDataPointmapMergeSettingDialog()
{
	delete ui;
}

void GeoDataPointmapMergeSettingDialog::setItems(const std::vector<PreProcessorGeoDataDataItemI*>& items)
{
	for (PreProcessorGeoDataDataItemI* item : items) {
		ui->listWidget->addItem(item->geoData()->caption());
	}
	m_items = items;
}

std::vector<PreProcessorGeoDataDataItemI*> GeoDataPointmapMergeSettingDialog::selectedItems() const
{
	std::vector<PreProcessorGeoDataDataItemI*> ret;
	for (int i = 0; i < ui->listWidget->count(); ++i) {
		auto item = ui->listWidget->item(i);
		if (! item->isSelected()) {continue;}

		ret.push_back(m_items.at(i));
	}
	return ret;
}
